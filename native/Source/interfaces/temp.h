#pragma once

#include <new>
#include <memory>
#include <cstdint>

namespace internal {
    void temp_free(void *mem) noexcept;

    [[nodiscard]] void *temp_allocate(uintptr_t size) noexcept;

    constexpr uintptr_t temp_max_span = 1u << 18u;
}

template<class T>
struct temp_alloc {
private:
    static constexpr uintptr_t align() noexcept {
        const auto trim = sizeof(T) / alignof(T) * alignof(T);
        return trim != sizeof(T) ? trim + alignof(T) : sizeof(T);
    }

    // Helper Const
    static constexpr uintptr_t alignment = alignof(T);
    static constexpr uintptr_t aligned_size = align();
    inline static std::allocator<T> default_alloc{};
public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using is_always_equal = std::true_type;
    using propagate_on_container_copy_assignment = std::false_type;
    using propagate_on_container_move_assignment = std::false_type;
    using propagate_on_container_swap = std::false_type;

    constexpr temp_alloc() noexcept = default;

    temp_alloc(temp_alloc &&) noexcept = default;

    temp_alloc &operator=(temp_alloc &&) noexcept = default;

    temp_alloc(const temp_alloc &) noexcept = default;

    temp_alloc &operator=(const temp_alloc &) noexcept = default;

    template<class U>
    explicit constexpr temp_alloc(const temp_alloc<U> &) noexcept {}

    T *address(T &x) const noexcept { return std::addressof(x); }

    const T *address(const T &x) const noexcept { return std::addressof(x); }

    [[nodiscard]] T *allocate(const std::size_t n) {
        if constexpr (alignment <= alignof(std::max_align_t)) {
            const auto size = n > 1 ? aligned_size * n : sizeof(T);
            if (size <= internal::temp_max_span) { return reinterpret_cast<T *>(internal::temp_allocate(size)); }
        }
        return default_alloc.allocate(n);
    }

    void deallocate(T *p, const std::size_t n) noexcept {
        if constexpr (alignment <= alignof(std::max_align_t)) {
            if ((n > 1 ? aligned_size * n : sizeof(T)) <= internal::temp_max_span) {
                return internal::temp_free(reinterpret_cast<void *>(const_cast<std::remove_cv_t<T> *>(p)));
            }
        }
        default_alloc.deallocate(p, n);
    }

    [[nodiscard]] bool operator==(const temp_alloc &r) const noexcept { return true; }

    [[nodiscard]] bool operator!=(const temp_alloc &r) const noexcept { return false; }
};
