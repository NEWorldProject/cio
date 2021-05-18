#include <jni.h>
#include <cstring>
#include "endian.h"

static void *ptr(const jlong mem) noexcept { return reinterpret_cast<void *>(static_cast<uintptr_t>(mem)); }

static void JNICALL copyB2N(JNIEnv *e, jclass, jbyteArray src, jlong dst, jint size) noexcept {
    const auto m = e->GetPrimitiveArrayCritical(src, nullptr);
    std::memmove(ptr(dst), m, size);
    e->ReleasePrimitiveArrayCritical(src, m, JNI_ABORT);
}

static void JNICALL copyN2B(JNIEnv *e, jclass, jlong src, jint size, jbyteArray dst) noexcept {
    const auto m = e->GetPrimitiveArrayCritical(dst, nullptr);
    std::memmove(m, ptr(src), size);
    e->ReleasePrimitiveArrayCritical(dst, m, 0);
}

static void JNICALL copyS2N(JNIEnv *e, jclass, jshortArray src, jlong dst, jint size, jboolean le) noexcept {
    const auto m = e->GetPrimitiveArrayCritical(src, nullptr);
    if (le && native_le) {
        std::memmove(ptr(dst), m, size);
    } else {
        auto l = static_cast<uint16_t *>(ptr(dst));
        auto r = static_cast<uint8_t *>(m);
        const auto end = l + size;
        for (; l < end; r += sizeof(uint16_t), ++l) *l = load16(r);
    }
    e->ReleasePrimitiveArrayCritical(src, m, JNI_ABORT);
}

static void JNICALL copyN2S(JNIEnv *e, jclass, jlong src, jint size, jshortArray dst, jboolean le) noexcept {
    const auto m = e->GetPrimitiveArrayCritical(dst, nullptr);
    if (le && native_le) {
        std::memmove(m, ptr(src), size);
    } else {
        auto l = static_cast<uint8_t *>(m);
        auto r = static_cast<uint16_t *>(ptr(src));
        const auto end = l + size;
        for (; l < end; r += sizeof(uint16_t), ++l) store16(*r, l);
    }
    e->ReleasePrimitiveArrayCritical(dst, m, 0);
}

static void JNICALL copyI2N(JNIEnv *e, jclass, jintArray src, jlong dst, jint size, jboolean le) noexcept {
    const auto m = e->GetPrimitiveArrayCritical(src, nullptr);
    if (le && native_le) {
        std::memmove(ptr(dst), m, size);
    } else {
        auto r = static_cast<uint8_t *>(m);
        auto l = static_cast<uint32_t *>(ptr(dst));
        const auto end = l + size;
        for (; l < end; r += sizeof(uint32_t), ++l) *l = load32(r);
    }
    e->ReleasePrimitiveArrayCritical(src, m, JNI_ABORT);
}

static void JNICALL copyN2I(JNIEnv *e, jclass, jlong src, jint size, jintArray dst, jboolean le) noexcept {
    const auto m = e->GetPrimitiveArrayCritical(dst, nullptr);
    if (le && native_le) {
        std::memmove(m, ptr(src), size);
    } else {
        auto l = static_cast<uint8_t *>(m);
        auto r = static_cast<uint32_t *>(ptr(src));
        const auto end = l + size;
        for (; l < end; r += sizeof(uint32_t), ++l) store32(*r, l);
    }
    e->ReleasePrimitiveArrayCritical(dst, m, 0);
}

static void JNICALL copyL2N(JNIEnv *e, jclass, jlongArray src, jlong dst, jint size, jboolean le) noexcept {
    const auto m = e->GetPrimitiveArrayCritical(src, nullptr);
    if (le && native_le) {
        std::memmove(ptr(dst), m, size);
    } else {
        auto r = static_cast<uint8_t *>(m);
        auto l = static_cast<uint64_t *>(ptr(dst));
        const auto end = l + size;
        for (; l < end; r += sizeof(uint64_t), ++l) *l = load64(r);
    }
    e->ReleasePrimitiveArrayCritical(src, m, JNI_ABORT);
}

static void JNICALL copyN2L(JNIEnv *e, jclass, jlong src, jint size, jlongArray dst, jboolean le) noexcept {
    const auto m = e->GetPrimitiveArrayCritical(dst, nullptr);
    if (le && native_le) {
        std::memmove(m, ptr(src), size);
    } else {
        auto l = static_cast<uint8_t *>(m);
        auto r = static_cast<uint64_t *>(ptr(src));
        const auto end = l + size;
        for (; l < end; r += sizeof(uint64_t), ++l) store64(*r, l);
    }
    e->ReleasePrimitiveArrayCritical(dst, m, 0);
}

static_assert(std::numeric_limits<float>::is_iec559);
static_assert(std::numeric_limits<double>::is_iec559);

template<class T>
static void *erase(T fn) noexcept { return reinterpret_cast<void *>(fn); }

// I do not know what is wrong with JNI developers, but this is the only way
static char *c(const char *s) noexcept { return const_cast<char *>(s); }

void registerArrayKt(JNIEnv *e) noexcept {
    const JNINativeMethod methods[] = {
            {c("copy"), c("([BJI)V"),  erase(copyB2N)},
            {c("copy"), c("(JI[B)V"),  erase(copyN2B)},
            {c("copy"), c("([SJIZ)V"), erase(copyS2N)},
            {c("copy"), c("(JI[SZ)V"), erase(copyN2S)},
            {c("copy"), c("([IJIZ)V"), erase(copyI2N)},
            {c("copy"), c("(JI[IZ)V"), erase(copyN2I)},
            {c("copy"), c("([JJIZ)V"), erase(copyL2N)},
            {c("copy"), c("(JI[JZ)V"), erase(copyN2L)},
            {c("copy"), c("([FJIZ)V"), erase(copyI2N)},
            {c("copy"), c("(JI[FZ)V"), erase(copyN2I)},
            {c("copy"), c("([DJIZ)V"), erase(copyL2N)},
            {c("copy"), c("(JI[DZ)V"), erase(copyN2L)}
    };
    const auto clazz = e->FindClass("site/neworld/cio/unsafe/ArrayKt");
    e->RegisterNatives(clazz, methods, 12);
}
