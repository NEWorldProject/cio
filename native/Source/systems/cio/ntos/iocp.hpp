#pragma once

#include <atomic>
#include "interfaces/cio.h"
#include "interfaces/temp.h"
#include "internal/system.h"

namespace internal {
    void ntos_iocp_file_register(HANDLE hdc);

    void ntos_iocp_register(HANDLE hdc);

    struct ntos_simple_file_aio_record;

    struct ntos_complex_file_aio_record final {
        std::atomic_int32_t countdown;
        uint32_t id;
        int32_t count;
        ntos_simple_file_aio_record *children;
        std::atomic<cio::status> first_error{cio::CIO_OK};

        ntos_complex_file_aio_record(uint32_t id, int32_t operations) noexcept;

        ~ntos_complex_file_aio_record() noexcept;
    };

    struct ntos_simple_file_aio_record final {
        OVERLAPPED overlapped{};
        ntos_complex_file_aio_record *complex;
        uint32_t id;

        explicit ntos_simple_file_aio_record(uint32_t id) noexcept: complex(nullptr), id(id) {}

        explicit ntos_simple_file_aio_record(ntos_complex_file_aio_record *complex) noexcept:
                complex(complex), id(0ul) {}

        void set_overlapped(uint64_t offset) noexcept {
            overlapped.Internal = 0ull;
            overlapped.InternalHigh = 0ull;
            overlapped.OffsetHigh = static_cast<uint32_t>(offset >> 32ull);
            overlapped.Offset = static_cast<uint32_t>(offset);
            overlapped.hEvent = nullptr;
        }
    };

    template<class FnCallback>
    class ntos_iocp_record final {
        static temp_alloc<ntos_iocp_record> g_allocator;
    public:
        static ntos_iocp_record* alloc(FnCallback &&fn) {
            return new(g_allocator.allocate(1)) ntos_iocp_record(std::forward<FnCallback>(fn));
        }

        OVERLAPPED overlapped{};
        LPOVERLAPPED_COMPLETION_ROUTINE p_fn = [](DWORD code, DWORD transferred, LPOVERLAPPED ov) noexcept {
            auto ths = reinterpret_cast<ntos_iocp_record *>(ov);
            ths->callback(code, transferred);
            ths->destruct();
        };
    private:
        FnCallback callback;

        explicit ntos_iocp_record(FnCallback &&fn): callback(std::forward<FnCallback>(fn)) {}

        void destruct() noexcept {
            this->~ntos_iocp_record();
            g_allocator.deallocate(this, 1);
        }
    };

    template<class FnCallback>
    inline temp_alloc<ntos_iocp_record<FnCallback>> ntos_iocp_record<FnCallback>::g_allocator;

    template<>
    class ntos_iocp_record<void> final {
    public:
        OVERLAPPED overlapped{0, 0, 0, 0, nullptr};
        LPOVERLAPPED_COMPLETION_ROUTINE p_fn = [](DWORD, DWORD, LPOVERLAPPED) noexcept {};
    };

    template<class FnCallback>
    inline LPOVERLAPPED ntos_bind_iocp_record(FnCallback &&fn) {
        return &(ntos_iocp_record<FnCallback>::alloc(std::forward<FnCallback>(fn))->overlapped);
    }

    void ntos_iocp_use_record(DWORD code, DWORD transferred, LPOVERLAPPED ov) noexcept;

    inline static temp_alloc<ntos_complex_file_aio_record> alloc_ntos_complex_aio;
    inline static temp_alloc<ntos_simple_file_aio_record> alloc_ntos_simple_aio;

    inline auto ntos_alloc_simple_aio(uint32_t id) noexcept {
        return new(alloc_ntos_simple_aio.allocate(1)) ntos_simple_file_aio_record(id);
    }

    inline auto ntos_alloc_complex_aio(uint32_t id, int32_t operations) noexcept {
        return new(alloc_ntos_complex_aio.allocate(1)) ntos_complex_file_aio_record(id, operations);
    }

    inline internal::ntos_complex_file_aio_record::ntos_complex_file_aio_record(uint32_t id,
                                                                                int32_t operations) noexcept:
            countdown(operations), id(id), count(operations), children(alloc_ntos_simple_aio.allocate(count)) {
        for (int32_t i = 0; i < count; ++i) children[i] = ntos_simple_file_aio_record(this);
    }

    inline ntos_complex_file_aio_record::~ntos_complex_file_aio_record() noexcept {
        alloc_ntos_simple_aio.deallocate(children, count);
    }

    void ntos_release_file_aio(ntos_simple_file_aio_record *record, uint64_t completed) noexcept;

    void ntos_release_aio(uint32_t id, int code, uint64_t completed = 0) noexcept;

    void ntos_release_aio(uint32_t id, const cio::exception_errc &error) noexcept;
}