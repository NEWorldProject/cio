#include "iocp.hpp"
#include "error.hpp"

using namespace cio;

namespace {
    void null_callback(uint32_t, uint32_t, uint32_t) noexcept {}

    completion_callback global_completion_callback = null_callback;

    void release_aio_record(internal::ntos_simple_aio_record *record, cio::status error, uint64_t completed) noexcept {
        if (record->complex == nullptr) {
            global_completion_callback(record->id, error, completed);
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
                complex->~ntos_complex_aio_record();
                internal::alloc_ntos_complex_aio.deallocate(complex, 1);
            }
        }
    }

    void ntos_iocp_callback(DWORD code, DWORD transferred, LPOVERLAPPED overlapped) noexcept {
        release_aio_record(
                reinterpret_cast<internal::ntos_simple_aio_record *>(overlapped),
                internal::translate_sys_error(code),
                transferred
        );
    }
}

void cio::set_completion_callback(completion_callback callback) noexcept {
    global_completion_callback = ((callback != nullptr) ? callback : null_callback);
}

void internal::iocp_register(HANDLE hdc) {
    if (!BindIoCompletionCallback(hdc, ntos_iocp_callback, 0)) {
        const auto code = GetLastError();
        throw exception_errc(internal::translate_sys_error(code));
    }
}

void internal::ntos_release_overlapped(
        internal::ntos_simple_aio_record *record, uint64_t completed
) noexcept {
    auto error = ((completed == 0ull) ? internal::translate_sys_error(GetLastError()) : CIO_OK);
    release_aio_record(record, error, completed);
}
