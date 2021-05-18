#pragma once

#include <bit>
#include <cstring>
#include <cstdint>

#ifdef _MSC_VER
#include <intrin.h>
#endif

constexpr bool native_le = std::endian::native == std::endian::little;

inline uint16_t load16(const uint8_t* v) noexcept {
    uint16_t ret;
    std::memcpy(&ret, v, sizeof(uint16_t));
#ifdef _MSC_VER
    return _byteswap_ushort(ret);
#else
    return __builtin_bswap16(ret);
#endif
}

inline void store16(const uint16_t l, uint8_t* r) noexcept {
#ifdef _MSC_VER
    const auto val = _byteswap_ushort(l);
#else
    const auto val = __builtin_bswap16(l);
#endif
    std::memcpy(r, &val, sizeof(uint16_t));
}

inline uint32_t load32(const uint8_t* v) noexcept {
    uint32_t ret;
    std::memcpy(&ret, v, sizeof(uint32_t));
#ifdef _MSC_VER
    return _byteswap_ushort(ret);
#else
    return __builtin_bswap32(ret);
#endif
}

inline void store32(const uint32_t l, uint8_t* r) noexcept {
#ifdef _MSC_VER
    const auto val = _byteswap_ulong(l);
#else
    const auto val = __builtin_bswap32(l);
#endif
    std::memcpy(r, &val, sizeof(uint32_t));
}

inline uint64_t load64(const uint8_t* v) noexcept {
    uint64_t ret;
    std::memcpy(&ret, v, sizeof(uint64_t));
#ifdef _MSC_VER
    return _byteswap_ushort(ret);
#else
    return __builtin_bswap64(ret);
#endif
}

inline void store64(const uint64_t l, uint8_t* r) noexcept {
#ifdef _MSC_VER
    const auto val = _byteswap_ushort(l);
#else
    const auto val = __builtin_bswap64(l);
#endif
    std::memcpy(r, &val, sizeof(uint64_t));
}
