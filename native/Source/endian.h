#pragma once

#include <bit>
#include <cstring>
#include <cstdint>

#ifdef _MSC_VER
#include <intrin.h>
#endif

constexpr bool native_le = std::endian::native == std::endian::little;

inline void load(uint16_t& ret, const uint8_t* v) noexcept {
    std::memcpy(&ret, v, sizeof(uint16_t));
#ifdef _MSC_VER
    ret = _byteswap_ushort(ret);
#else
    ret = __builtin_bswap16(ret);
#endif
}

inline void load(uint32_t& ret, const uint8_t* v) noexcept {
    std::memcpy(&ret, v, sizeof(uint32_t));
#ifdef _MSC_VER
    ret = _byteswap_ulong(ret);
#else
    ret = __builtin_bswap32(ret);
#endif
}

inline void load(uint64_t& ret, const uint8_t* v) noexcept {
    std::memcpy(&ret, v, sizeof(uint64_t));
#ifdef _MSC_VER
    ret = _byteswap_uint64(ret);
#else
    ret = __builtin_bswap64(ret);
#endif
}

inline void store(uint8_t* l, const uint16_t r) noexcept {
#ifdef _MSC_VER
    const auto v = _byteswap_ushort(r);
#else
    const auto v = __builtin_bswap16(l);
#endif
    std::memcpy(l, &v, sizeof(uint16_t));
}

inline void store(uint8_t* l, const uint32_t r) noexcept {
#ifdef _MSC_VER
    const auto v = _byteswap_ulong(r);
#else
    const auto v = __builtin_bswap32(l);
#endif
    std::memcpy(l, &v, sizeof(uint32_t));
}

inline void store(uint8_t* l, const uint64_t r) noexcept {
#ifdef _MSC_VER
    const auto v = _byteswap_uint64(r);
#else
    const auto v = __builtin_bswap64(l);
#endif
    std::memcpy(l, &v, sizeof(uint64_t));
}
