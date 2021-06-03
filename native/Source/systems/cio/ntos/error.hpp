#pragma once

#include "interfaces/cio.h"
#include "internal/system.h"

namespace internal {
    cio::status translate_sys_error(DWORD sys) noexcept;
}