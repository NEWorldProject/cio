#pragma once

#include <atomic>
#include <memory>
#include "block.h"

struct Header final {
    std::atomic_int32_t flying{0};
};

static Header *fetch() noexcept { return new(getBlock()->data) Header; }

static void release(Header *const blk) noexcept { returnBlock(reinterpret_cast<Block *>(blk)); }

[[nodiscard]] static constexpr uintptr_t maxAlign(const uintptr_t size) noexcept {
    constexpr auto mask = alignof(std::max_align_t) - 1;
    constexpr auto rev = ~mask;
    if (size & mask) return (size & rev) + alignof(std::max_align_t); else return size;
}

static constexpr auto alignedHeaderSize = maxAlign(sizeof(Header));

class Allocation final {
public:
    [[nodiscard]] bool flush(Header *&out) const noexcept {
        if (current) {
            out = current;
            return (current->flying.fetch_add(count) == -count);
        }
        return false;
    }

    void reset(Header *const other) noexcept {
        current = other, head = reinterpret_cast<uintptr_t>(current) + alignedHeaderSize, count = 0;
    }

    [[nodiscard]] void *allocate(const uintptr_t size) noexcept {
        const auto aligned = maxAlign(size);
        if (const auto expected = head + aligned; expected < BlockEnd) {
            ++count;
            const auto res = reinterpret_cast<uintptr_t>(current) + head;
            head = expected;
            return reinterpret_cast<void *>(res);
        }
        return nullptr;
    }

private:
    Header *current{};
    uintptr_t head{};
    int32_t count{};
};

class PerThread final {
    Allocation alloc{};

    void reset(Header *const next = fetch()) noexcept {
        if (Header *last = nullptr; alloc.flush(last)) {
            if (last) release(last);
        }
        alloc.reset(next);
    }

public:
    PerThread() noexcept { reset(); }

    ~PerThread() noexcept { reset(nullptr); }

    [[nodiscard]] static void *allocate(const uintptr_t size) noexcept {
        static const thread_local auto o = std::make_unique<PerThread>();
        for (;;) if (const auto ret = o->alloc.allocate(size); ret) return ret; else o->reset();
    }

    static void free(void *const mem) noexcept {
        static constexpr uintptr_t rev = 0b11'1111'1111'1111'1111'1111;
        static constexpr uintptr_t mask = ~rev;
        const auto base = reinterpret_cast<uintptr_t>(mem) & mask;
        const auto header = reinterpret_cast<Header *>(base);
        if (header->flying.fetch_sub(1) == 1) release(header);
    }
};