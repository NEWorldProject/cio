#include <cstring>
#include "endian.h"
#include "interfaces/jni.h"

static void JNICALL copyB2N(JNIEnv *e, jclass, jbyteArray src, jlong dst, jint size) noexcept {
    const auto c_src = e->GetPrimitiveArrayCritical(src, nullptr);
    std::memmove(ptr(dst), c_src, size);
    e->ReleasePrimitiveArrayCritical(src, c_src, JNI_ABORT);
}

static void JNICALL copyN2B(JNIEnv *e, jclass, jlong src, jint size, jbyteArray dst) noexcept {
    const auto c_dst = e->GetPrimitiveArrayCritical(dst, nullptr);
    std::memmove(c_dst, ptr(src), size);
    e->ReleasePrimitiveArrayCritical(dst, c_dst, 0);
}

template<class T>
static void JNICALL copyM2U(JNIEnv *e, jclass, jarray src, jlong dst, jint size, jboolean le) noexcept {
    const auto c_src = e->GetPrimitiveArrayCritical(src, nullptr);
    if (le && native_le) {
        std::memmove(ptr(dst), c_src, size * sizeof(T));
    } else {
        auto l = static_cast<uint8_t *>(ptr(dst));
        auto r = static_cast<T *>(c_src);
        const auto end = r + size;
        for (; r < end; ++r, l += sizeof(T)) store(l, *r);
    }
    e->ReleasePrimitiveArrayCritical(src, c_src, JNI_ABORT);
}

template<class T>
static void JNICALL copyU2M(JNIEnv *e, jclass, jlong src, jint size, jarray dst, jboolean le) noexcept {
    const auto c_dst = e->GetPrimitiveArrayCritical(dst, nullptr);
    if (le && native_le) {
        std::memmove(c_dst, const_cast<const void *>(ptr(src)), size * sizeof(T));
    } else {
        auto l = static_cast<T *>(c_dst);
        auto r = static_cast<uint8_t *>(ptr(src));
        const auto end = l + size;
        for (; l < end; ++l, r += sizeof(T)) load(*l, r);
    }
    e->ReleasePrimitiveArrayCritical(dst, c_dst, 0);
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
