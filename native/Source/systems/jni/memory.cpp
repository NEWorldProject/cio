#include <cstdlib>
#include <cstring>
#include "interfaces/jni.h"
#include "interfaces/temp.h"

static void JNICALL heapFree(JNIEnv *, jclass, jlong hdc, jlong) noexcept { std::free(ptr(hdc)); }

static jlong JNICALL heapAllocate(JNIEnv *, jclass, jlong size) noexcept { return hdc(std::malloc(size)); }

static jlong JNICALL heapRelocate(JNIEnv *, jclass, jlong h, jlong, jlong newSize) noexcept {
    return hdc(std::realloc(ptr(h), newSize));
}

static void tempFreeI(void *mem, jlong size) noexcept {
    if (size <= internal::temp_max_span) internal::temp_free(mem); else std::free(mem);
}

static void *tempAllocateI(jlong size) noexcept {
    if (size <= internal::temp_max_span) return internal::temp_allocate(size);
    return std::malloc(size);
}

static void JNICALL tempFree(JNIEnv *, jclass, jlong h, jlong size) noexcept { tempFreeI(ptr(h), size); }

static jlong JNICALL tempAllocate(JNIEnv *, jclass, jlong size) noexcept { return hdc(tempAllocateI(size)); }

static jlong JNICALL tempRelocate(JNIEnv *, jclass, jlong h, jlong oldSize, jlong newSize) noexcept {
    // skip two situations where we have a fast path
    if (oldSize <= internal::temp_max_span && newSize <= oldSize) return h;
    if (oldSize > internal::temp_max_span && newSize > internal::temp_max_span)
        return hdc(std::realloc(ptr(h), newSize));
    // temp_allocate a new segment
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
            {mut("tempFree"),     mut("(JJ)V"),  erase(tempFree)},
            {mut("tempAllocate"), mut("(J)J"),   erase(tempAllocate)},
            {mut("tempRelocate"), mut("(JJJ)J"), erase(tempRelocate)},
            {mut("heapFree"),     mut("(JJ)V"),  erase(heapFree)},
            {mut("heapAllocate"), mut("(J)J"),   erase(heapAllocate)},
            {mut("heapRelocate"), mut("(JJJ)J"), erase(heapRelocate)},
    };
    const auto clazz = e->FindClass("site/neworld/cio/unsafe/MemoryKt");
    e->RegisterNatives(clazz, methods, 6);
}
