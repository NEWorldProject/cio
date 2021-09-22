#include <memory>
#include "cio_jni.h"
#include "interfaces/temp.h"

namespace cb = cio::block;

static jlong JNICALL open(JNIEnv *e, jclass, jbyteArray path, jint flags) noexcept {
    const auto bytes = jni::get_critical_array<0, char>(e, path);
    temp_string string(bytes.get(), e->GetArrayLength(path));
    return static_cast<jlong>(cb::open(string.c_str(), flags));
}

static void JNICALL close(JNIEnv *, jclass, jlong handle) noexcept { cb::close(handle); }

static void
JNICALL read(JNIEnv *, jclass, jint dispatch, jlong handle, jlong buffer, jlong offset, jlong length) noexcept {
    cb::read(handle, dispatch, buffer, length, offset);
}

static void
JNICALL write(JNIEnv *, jclass, jint dispatch, jlong handle, jlong buffer, jlong offset, jlong length) noexcept {
    cb::write(handle, dispatch, buffer, length, offset);
}

static void check_length(JNIEnv *e, jlongArray a, jlongArray b) {
    if (e->GetArrayLength(a) != e->GetArrayLength(b)) throw cio::exception_errc(cio::CIO_EINVAL);
}

static void JNICALL readMulti(
        JNIEnv *e, jclass, jint dispatch, jlong handle,
        jlongArray buffers, jlongArray sizes, jlongArray offsets, jlongArray spans
) noexcept {
    cio::jni::guarded_execute(dispatch, [&]() {
        check_length(e, buffers, sizes);
        check_length(e, offsets, spans);
        const auto c_buffers = jni::get_critical_array<0, uint64_t>(e, buffers);
        const auto c_sizes = jni::get_critical_array<0, uint64_t>(e, sizes);
        const auto c_offsets = jni::get_critical_array<0, uint64_t>(e, offsets);
        const auto c_spans = jni::get_critical_array<0, uint64_t>(e, spans);
        cb::readv(handle, dispatch, c_buffers.get(), c_sizes.get(), c_offsets.get(), c_spans.get());
    });
}

static void JNICALL writeMulti(
        JNIEnv *e, jclass, jint dispatch, jlong handle,
        jlongArray buffers, jlongArray sizes, jlongArray offsets, jlongArray spans
) noexcept {
    cio::jni::guarded_execute(dispatch, [&]() {
        check_length(e, buffers, sizes);
        check_length(e, offsets, spans);
        const auto c_buffers = jni::get_critical_array<0, uint64_t>(e, buffers);
        const auto c_sizes = jni::get_critical_array<0, uint64_t>(e, sizes);
        const auto c_offsets = jni::get_critical_array<0, uint64_t>(e, offsets);
        const auto c_spans = jni::get_critical_array<0, uint64_t>(e, spans);
        cb::writev(handle, dispatch, c_buffers.get(), c_sizes.get(), c_offsets.get(), c_spans.get());
    });
}

void registerBlockKt(JNIEnv *e) noexcept {
    const JNINativeMethod methods[] = {
            {mut("open"),       mut("([BI)J"),        erase(open)},
            {mut("close"),      mut("(J)V"),          erase(close)},
            {mut("read"),       mut("(IJJJJ)V"),       erase(read)},
            {mut("write"),      mut("(IJJJJ)V"),       erase(write)},
            {mut("readMulti"),  mut("(IJ[J[J[J[J)V"), erase(readMulti)},
            {mut("writeMulti"), mut("(IJ[J[J[J[J)V"), erase(writeMulti)},
    };
    const auto clazz = e->FindClass("site/neworld/cio/block/BlockKt");
    e->RegisterNatives(clazz, methods, 6);
}