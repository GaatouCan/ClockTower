#pragma once

#include "../../base/ISubSystem.h"

namespace base {
    class DatabaseSystem final : public ISubSystem {
        SUB_SYSTEM_BODY(DatabaseSystem)
        void init() override;
    };
} // base
