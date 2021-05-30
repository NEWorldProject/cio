#pragma once

#include <jni.h>

template<class T>
inline void *erase(T fn) noexcept { return reinterpret_cast<void *>(fn); }

// I do not know what is wrong with JNI developers, but this is the only way
inline char *c(const char *s) noexcept { return const_cast<char *>(s); }

inline void *ptr(const jlong mem) noexcept { return reinterpret_cast<void *>(static_cast<uintptr_t>(mem)); }

inline jlong hdc(void *const mem) noexcept { return static_cast<jlong>(reinterpret_cast<uintptr_t>(mem)); }
