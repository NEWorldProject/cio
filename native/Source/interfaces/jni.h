#pragma once

#include <jni.h>
#include <memory>

template<class T>
inline void *erase(T fn) noexcept { return reinterpret_cast<void *>(fn); }

// I do not know what is wrong with JNI developers, but this is the only way
inline char *mut(const char *s) noexcept { return const_cast<char *>(s); }

inline void *ptr(const jlong mem) noexcept { return reinterpret_cast<void *>(static_cast<uintptr_t>(mem)); }

inline jlong hdc(void *const mem) noexcept { return static_cast<jlong>(reinterpret_cast<uintptr_t>(mem)); }

namespace jni {
    class resource_error: public std::exception {

    };

    namespace internal {
        template<jint mode, class T>
        struct ca_release {
            JNIEnv *e;
            jarray array;

            void operator()(T *p) const noexcept { e->ReleasePrimitiveArrayCritical(array, p, mode); }
        };
    }

    template<jint mode, class T>
    using critical_array = std::unique_ptr<T[], internal::ca_release<mode, T>>;

    template<jint mode, class T>
    inline critical_array<mode, T> get_critical_array(JNIEnv *e, jarray array) {
        const auto arr = e->GetPrimitiveArrayCritical(array, nullptr);
        if (!arr) throw std::bad_alloc();
        return critical_array<mode, T>(reinterpret_cast<T *>(arr), {e, array});
    }
}