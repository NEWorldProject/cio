#include <cstdlib>
#include <cstring>
#include <cstdint>
#include "shared.h"
#include "temp/allocation.h"

static constexpr uintptr_t AllocThreshold = 1u << 18u;

static void JNICALL heapFree(JNIEnv *, jclass, jlong hdc, jlong) noexcept { std::free(ptr(hdc)); }

static jlong JNICALL heapAllocate(JNIEnv *, jclass, jlong size) noexcept { return hdc(std::malloc(size)); }

static jlong JNICALL heapRelocate(JNIEnv *, jclass, jlong h, jlong, jlong newSize) noexcept {
    return hdc(std::realloc(ptr(h), newSize));
}

static void tempFreeI(void *mem, jlong size) noexcept {
    if (size <= AllocThreshold) PerThread::free(mem); else std::free(mem);
}

static void *tempAllocateI(jlong size) noexcept {
    if (size <= AllocThreshold) return PerThread::allocate(size);
    return std::malloc(size);
}

static void JNICALL tempFree(JNIEnv *, jclass, jlong h, jlong size) noexcept { tempFreeI(ptr(h), size); }

static jlong JNICALL tempAllocate(JNIEnv *, jclass, jlong size) noexcept { return hdc(tempAllocateI(size)); }

static jlong JNICALL tempRelocate(JNIEnv *, jclass, jlong h, jlong oldSize, jlong newSize) noexcept {
    // skip two situations where we have a fast path
    if (oldSize <= AllocThreshold && newSize <= oldSize) return h;
    if (oldSize > AllocThreshold && newSize > AllocThreshold) return hdc(std::realloc(ptr(h), newSize));
    // allocate a new segment
    const auto segment = tempAllocateI(newSize);
    // early return on failure
    if (!segment) return 0ll;
    std::memcpy(segment, ptr(h), oldSize);
    // release the old segment
    tempFreeI(ptr(h), oldSize);
    return hdc(segment);
}

void registerMemoryKt(JNIEnv *e) noexcept {
    const JNINativeMethod methods[] = {
            {c("tempFree"),      c("(JJ)V"),  erase(tempFree)},
            {c("tempAllocateI"), c("(J)J"),   erase(tempAllocate)},
            {c("tempRelocate"),  c("(JJJ)J"), erase(tempRelocate)},
            {c("heapFree"),      c("(JJ)V"),  erase(heapFree)},
            {c("heapAllocate"),  c("(J)J"),   erase(heapAllocate)},
            {c("heapRelocate"),  c("(JJJ)J"), erase(heapRelocate)},
    };
    const auto clazz = e->FindClass("site/neworld/cio/unsafe/MemoryKt");
    e->RegisterNatives(clazz, methods, 6);
}
