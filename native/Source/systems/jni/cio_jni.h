#pragma once

#include "interfaces/jni.h"
#include "interfaces/cio.h"

namespace cio::jni {
    void direct_dispatch(uint32_t id, uint32_t status, uint32_t completed_bytes) noexcept;

    template<class F>
    void guarded_execute(uint32_t id, F func) noexcept {
        try {
            func();
        }
        catch (cio::exception_errc &e) {
            cio::jni::direct_dispatch(id, e.errc, 0);
        }
        catch (...) {
            cio::jni::direct_dispatch(id, cio::CIO_EINTR, 0);
        }
    }
}