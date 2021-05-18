#include <jni.h>

// setup functions from components
void registerArrayKt(JNIEnv *e) noexcept;

jint JNICALL JNI_OnLoad(JavaVM *vm, void *) {
    JNIEnv *e;
    vm->GetEnv(reinterpret_cast<void **>(&e), JNI_VERSION_1_8);
    registerArrayKt(e);
    return JNI_VERSION_1_8;
}
