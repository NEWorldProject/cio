#pragma once

#include <cstdint>

namespace internal {
    static constexpr uintptr_t block_size = 4u << 20u; // 4MiB

    void* rent_block() noexcept;

    void return_block(void* blk) noexcept;
}
