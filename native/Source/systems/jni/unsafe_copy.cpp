#include <cstring>
#include "endian.h"
#include "interfaces/jni.h"

static void JNICALL copyB2N(JNIEnv *e, jclass, jbyteArray src, jlong dst, jint size) noexcept {
    const auto c_src = jni::get_critical_array<JNI_ABORT, jbyte>(e, src);
    std::memmove(ptr(dst), c_src.get(), size);
}

static void JNICALL copyN2B(JNIEnv *e, jclass, jlong src, jint size, jbyteArray dst) noexcept {
    const auto c_dst = jni::get_critical_array<0, jlong>(e, dst);
    std::memmove(c_dst.get(), ptr(src), size);
}

template<class T>
static void JNICALL copyM2U(JNIEnv *e, jclass, jarray src, jlong dst, jint size, jboolean le) noexcept {
    const auto c_src = jni::get_critical_array<JNI_ABORT, T>(e, src);
    if (le && native_le) {
        std::memmove(ptr(dst), c_src.get(), size * sizeof(T));
    } else {
        auto l = static_cast<uint8_t *>(ptr(dst));
        auto r = c_src.get();
        const auto end = r + size;
        for (; r < end; ++r, l += sizeof(T)) store(l, *r);
    }
}

template<class T>
static void JNICALL copyU2M(JNIEnv *e, jclass, jlong src, jint size, jarray dst, jboolean le) noexcept {
    const auto c_dst = jni::get_critical_array<0, T>(e, dst);
    if (le && native_le) {
        std::memmove(c_dst.get(), const_cast<const void *>(ptr(src)), size * sizeof(T));
    } else {
        auto l = c_dst.get();
        auto r = static_cast<uint8_t *>(ptr(src));
        const auto end = l + size;
        for (; l < end; ++l, r += sizeof(T)) load(*l, r);
    }
}

static_assert(std::numeric_limits<float>::is_iec559);
static_assert(std::numeric_limits<double>::is_iec559);

void registerArrayKt(JNIEnv *e) noexcept {
    static char *const name = mut("copy");
    const JNINativeMethod methods[] = {
            {name, mut("([BJI)V"),  erase(copyB2N)},
            {name, mut("(JI[B)V"),  erase(copyN2B)},
            {name, mut("([SJIZ)V"), erase(copyM2U<uint16_t>)},
            {name, mut("(JI[SZ)V"), erase(copyU2M<uint16_t>)},
            {name, mut("([IJIZ)V"), erase(copyM2U<uint32_t>)},
            {name, mut("(JI[IZ)V"), erase(copyU2M<uint32_t>)},
            {name, mut("([JJIZ)V"), erase(copyM2U<uint64_t>)},
            {name, mut("(JI[JZ)V"), erase(copyU2M<uint64_t>)},
            {name, mut("([FJIZ)V"), erase(copyM2U<uint32_t>)},
            {name, mut("(JI[FZ)V"), erase(copyU2M<uint32_t>)},
            {name, mut("([DJIZ)V"), erase(copyM2U<uint64_t>)},
            {name, mut("(JI[DZ)V"), erase(copyU2M<uint64_t>)}
    };
    const auto clazz = e->FindClass("site/neworld/cio/unsafe/ArrayKt");
    e->RegisterNatives(clazz, methods, 12);
}
