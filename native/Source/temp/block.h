#pragma once

#include <new>
#include <cstdlib>

static constexpr uintptr_t BlockSize = 4u << 20u; // 4MiB
static constexpr uintptr_t BlockEnd = BlockSize - 64; // Reverse the last cache line for housekeeping

struct Block {
    char data[BlockEnd]{};
    char reserved[64]{};
};

#if __has_include(<Windows.h>)
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

#define SYSTEM_WINDOWS
namespace {
    struct HouseKeep {
        uintptr_t Base = 0;
    };
}
#else
#define SYSTEM_POSIX
#endif

[[nodiscard]] static Block *getBlock() noexcept {
#ifdef SYSTEM_POSIX
    void* mem;
    const int ret = posix_memalign(&mem, BlockSize, BlockSize);
    return new(mem) Block;
#elif defined(SYSTEM_WINDOWS)
    static constexpr auto mask = BlockSize - 1;
    static constexpr auto rev = ~mask;
    const auto base = reinterpret_cast<uintptr_t>(VirtualAlloc(nullptr, BlockSize << 1, MEM_RESERVE,
                                                               PAGE_READWRITE));
    const auto loc = base & mask ? (base & rev) + BlockSize : base;
    const auto blk = new(VirtualAlloc(reinterpret_cast<LPVOID>(loc), BlockSize, MEM_COMMIT, PAGE_READWRITE)) Block;
    const auto kp = reinterpret_cast<HouseKeep *>(blk->reserved);
    kp->Base = base;
    return blk;
#endif
}

static void returnBlock(Block *const blk) noexcept {
#ifdef SYSTEM_POSIX
    free(blk);
#elif defined(SYSTEM_WINDOWS)
    VirtualFree(reinterpret_cast<LPVOID>(reinterpret_cast<HouseKeep *>(blk->reserved)->Base), 0, MEM_RELEASE);
#endif
}
