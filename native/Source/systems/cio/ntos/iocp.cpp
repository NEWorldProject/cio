#include "iocp.hpp"
#include "error.hpp"

using namespace cio;

namespace {
    void null_callback(uint32_t, uint32_t, uint32_t) noexcept {}

    completion_callback global_completion_callback = null_callback;

    void release_aio_record(internal::ntos_simple_file_aio_record *record, status error, uint64_t completed) noexcept {
        if (record->complex == nullptr) {
            global_completion_callback(record->id, error, completed);
            record->~ntos_simple_file_aio_record();
            internal::alloc_ntos_simple_aio.deallocate(record, 1);
        } else {
            const auto complex = record->complex;
            const auto remaining = complex->countdown.fetch_sub(1) - 1;
            if (error != CIO_OK) {
                auto xchg = cio::CIO_OK;
                complex->first_error.compare_exchange_strong(xchg, error);
            }
            if (remaining == 0) {
                global_completion_callback(complex->id, complex->first_error, completed);
                complex->~ntos_complex_file_aio_record();
                internal::alloc_ntos_complex_aio.deallocate(complex, 1);
            }
        }
    }

    void ntos_file_iocp_callback(DWORD code, DWORD transferred, LPOVERLAPPED overlapped) noexcept {
        release_aio_record(
                reinterpret_cast<internal::ntos_simple_file_aio_record *>(overlapped),
                internal::translate_sys_error(code),
                transferred
        );
    }
}

void cio::set_completion_callback(completion_callback callback) noexcept {
    global_completion_callback = ((callback != nullptr) ? callback : null_callback);
}

void internal::ntos_iocp_file_register(HANDLE hdc) {
    if (!BindIoCompletionCallback(hdc, ntos_file_iocp_callback, 0)) {
        const auto code = GetLastError();
        throw exception_errc(internal::translate_sys_error(code));
    }
}

void internal::ntos_iocp_register(HANDLE hdc) {
    if (!BindIoCompletionCallback(hdc, ntos_iocp_use_record, 0)) {
        const auto code = GetLastError();
        throw exception_errc(internal::translate_sys_error(code));
    }
}

void internal::ntos_release_file_aio(
        internal::ntos_simple_file_aio_record *record, uint64_t completed
) noexcept {
    auto error = ((completed == 0ull) ? internal::translate_sys_error(GetLastError()) : CIO_OK);
    release_aio_record(record, error, completed);
}

void internal::ntos_iocp_use_record(DWORD code, DWORD transferred, LPOVERLAPPED ov) noexcept {
    auto rec = reinterpret_cast<internal::ntos_iocp_record<void> *>(ov);
    rec->p_fn(code, transferred, ov);
}

void internal::ntos_release_aio(uint32_t id, int code, uint64_t completed) noexcept {
    auto error = ((code == ERROR_SUCCESS) ? internal::translate_sys_error(code) : CIO_OK);
    global_completion_callback(id, error, completed);
}

void internal::ntos_release_aio(uint32_t id, const exception_errc &error) noexcept {
    global_completion_callback(id, error.errc, 0);
}
