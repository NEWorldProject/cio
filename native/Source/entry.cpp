#include <jni.h>

// setup functions from components
void registerArrayKt(JNIEnv *e) noexcept;
void registerMemoryKt(JNIEnv *e) noexcept;
void loadIODispatchKt(JavaVM *vm, JNIEnv *e);
void unloadIODispatchKt() noexcept;
void registerBlockKt(JNIEnv *e) noexcept;

jint JNICALL JNI_OnLoad(JavaVM *vm, void *) {
    JNIEnv *e;
    vm->GetEnv(reinterpret_cast<void **>(&e), JNI_VERSION_1_8);
    registerArrayKt(e);
    registerMemoryKt(e);
    loadIODispatchKt(vm, e);
    registerBlockKt(e);
    return JNI_VERSION_1_8;
}

void JNICALL JNI_OnUnload(JavaVM *vm, void *) {
    unloadIODispatchKt();
}
