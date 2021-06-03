#include "iocp.hpp"
#include "error.hpp"

using namespace cio;

namespace {
    HANDLE ntos_create_io_completion_port() {
        const auto port = CreateIoCompletionPort(nullptr, nullptr, 0, 0);
        if (port == nullptr) throw exception_errc(internal::translate_sys_error(GetLastError()));
        return port;
    }

    HANDLE ntos_get_iocp() {
        static HANDLE port = ntos_create_io_completion_port();
        return port;
    }

    void null_callback(uint32_t, uint32_t, uint32_t) noexcept {}

    completion_callback global_completion_callback = null_callback;
}

void internal::iocp_register(HANDLE hdc, uintptr_t key) {
    const auto port = CreateIoCompletionPort(hdc, ntos_get_iocp(), key, 0);
    if (port == nullptr) throw exception_errc(internal::translate_sys_error(GetLastError()));
}

void cio::set_completion_callback(completion_callback callback) noexcept {
    global_completion_callback = ((callback != nullptr) ? callback : null_callback);
}

void internal::ntos_release_overlapped(internal::ntos_simple_aio_record *record, uint64_t completed) noexcept {
    auto error = ((completed == 0ull) ? internal::translate_sys_error(GetLastError()) : CIO_OK);
    if (record->complex == nullptr) {
        global_completion_callback(record->id, error, completed);
        alloc_ntos_simple_aio.deallocate(record, 1);
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
            alloc_ntos_complex_aio.deallocate(complex, 1);
        }
    }
}

