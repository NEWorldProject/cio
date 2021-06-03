#pragma once

#include <atomic>
#include "interfaces/cio.h"
#include "interfaces/temp.h"
#include "internal/system.h"

namespace internal {
    void iocp_register(HANDLE hdc, uintptr_t key);

    struct ntos_simple_aio_record;

    struct ntos_complex_aio_record final {
        std::atomic_int32_t countdown;
        uint32_t id;
        int32_t count;
        ntos_simple_aio_record *children;
        std::atomic<cio::status> first_error{cio::CIO_OK};

        ntos_complex_aio_record(uint32_t id, int32_t operations) noexcept;

        ~ntos_complex_aio_record() noexcept;
    };

    struct ntos_simple_aio_record final {
        OVERLAPPED overlapped{};
        ntos_complex_aio_record *complex;
        uint32_t id;

        explicit ntos_simple_aio_record(uint32_t id) noexcept: complex(nullptr), id(id) {}

        explicit ntos_simple_aio_record(ntos_complex_aio_record *complex) noexcept:
                complex(complex), id(0ul) {}

        void set_overlapped(uint64_t offset) noexcept {
            overlapped.Internal = 0ull;
            overlapped.InternalHigh = 0ull;
            overlapped.OffsetHigh = static_cast<uint32_t>(offset >> 32ull);
            overlapped.Offset = static_cast<uint32_t>(offset);
            overlapped.hEvent = nullptr;
        }
    };

    inline static temp_alloc<ntos_complex_aio_record> alloc_ntos_complex_aio;
    inline static temp_alloc<ntos_simple_aio_record> alloc_ntos_simple_aio;

    inline auto ntos_alloc_simple_aio(uint32_t id) noexcept {
        auto ptr = alloc_ntos_simple_aio.allocate(1);
        new(ptr) ntos_simple_aio_record(id);
        return ptr;
    }

    inline auto ntos_alloc_complex_aio(uint32_t id, int32_t operations) noexcept {
        auto ptr = alloc_ntos_complex_aio.allocate(1);
        new(ptr) ntos_complex_aio_record(id, operations);
        return ptr;
    }

    inline internal::ntos_complex_aio_record::ntos_complex_aio_record(uint32_t id, int32_t operations) noexcept:
            countdown(operations), id(id), count(operations), children(alloc_ntos_simple_aio.allocate(count)) {
        for (int32_t i = 0; i < count; ++i) children[i] = ntos_simple_aio_record(this);
    }

    inline ntos_complex_aio_record::~ntos_complex_aio_record() noexcept {
        alloc_ntos_simple_aio.deallocate(children, count);
    }

    void ntos_release_overlapped(ntos_simple_aio_record *record, uint64_t completed) noexcept;
}