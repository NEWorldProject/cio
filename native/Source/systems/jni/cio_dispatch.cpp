#include <mutex>
#include <atomic>
#include <algorithm>
#include "internal/semaphore.h"
#include "interfaces/jni.h"
#include "interfaces/cio.h"
#include "interfaces/temp.h"

namespace {
    struct complete_data {
        uint32_t id;
        uint32_t status;
        uint32_t completed_bytes;
    };

    struct completion_segment;

    constexpr int record_count = (internal::block_size - sizeof(uintptr_t)) / sizeof(complete_data);

    struct completion_segment {
        completion_segment *next;
        complete_data data[record_count];
    };

    temp_alloc<completion_segment> alloc_segment;

    completion_segment *get_block() noexcept {
        auto ptr = alloc_segment.allocate(1);
        ptr->next = nullptr;
        return ptr;
    }

    int rotation_head = 0;
    std::atomic_int queue_counter{0};
    std::atomic_int rotation_tail{0};
    std::mutex block_lock;
    completion_segment *head = get_block();
    std::atomic<completion_segment *> tail = head;
    semaphore activation;
    std::atomic_bool activated{false};

    void completion_callback(uint32_t id, uint32_t status, uint32_t completed_bytes) noexcept {
        auto block = tail.load();
        auto current = rotation_tail.fetch_add(1);
        if (current >= record_count) {
            const std::lock_guard lk{block_lock};
            const auto current2 = rotation_tail.load();
            // we use this as the double locking mechanism.
            // this only works under the assumption that each block holds around 350K records
            // so under the current computer system, it is very unlikely that such number of io requests
            // can be completed in the time-span of sub-millisecond.
            if (current2 >= current) {
                const auto new_block = get_block();
                block = block->next = new_block;
                tail.store(new_block);
                current = 0, rotation_tail.store(1);
            }
        }
        block->data[current] = {id, status, completed_bytes};
        // update and check the queue counter
        const auto last_counter = queue_counter.fetch_add(1);
        if (last_counter == 0) activation.signal();
    }

    void dispatch_worker(JNIEnv *e, jobject dispatch, jmethodID method) noexcept {
        while (activation.wait(), activated.load()) {
            for (;;) {
                const auto max = record_count - rotation_head;
                const auto ready = queue_counter.load();
                const auto count = std::min(ready, max);
                const auto mem_head = reinterpret_cast<uintptr_t>(head->data + rotation_head);
                e->CallVoidMethod(dispatch, method, jlong(mem_head), jlong(count));
                if (queue_counter.fetch_sub(count) == count) break;
            }
        }
    }

    jobject get_dispatch_object(JNIEnv *e) {
        const auto clazz = e->FindClass("site/neworld/cio/internal/IODispatchKt");
        const auto method = e->GetMethodID(
                clazz,
                "getDispatch",
                "()Lsite/neworld/cio/internal/CIONativeDispatch;"
        );
        return e->NewGlobalRef(e->CallStaticObjectMethod(clazz, method));
    }

    jmethodID get_dispatch_method(JNIEnv *e) {
        const auto clazz = e->FindClass("site/neworld/cio/internal/CIONativeDispatch");
        return e->GetMethodID(clazz, "batchDispatch", "(JJ)V;");
    }
}

void loadIODispatchKt(JavaVM *vm, JNIEnv *e) {
    cio::set_completion_callback(completion_callback);
    // fetch the instance
    const auto dispatch = get_dispatch_object(e);
    const auto dispatch_method = get_dispatch_method(e);
    // launch worker
    activated.store(true);
    std::thread([=]() noexcept {
        JNIEnv *thread_env{};
        vm->AttachCurrentThread(reinterpret_cast<void **>(&thread_env), nullptr);
        dispatch_worker(thread_env, dispatch, dispatch_method);
        thread_env->DeleteGlobalRef(dispatch);
        vm->DetachCurrentThread();
    }).detach();
}

void unloadIODispatchKt() noexcept {
    cio::set_completion_callback(nullptr); // null the callback
    activated.store(false); // stop the feedback loop
}
