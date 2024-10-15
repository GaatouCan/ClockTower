#pragma once

#include "../../base/ISubSystem.h"

namespace base {
    class ManagerSystem final : public ISubSystem {
        SUB_SYSTEM_BODY(ManagerSystem)

        void init() override;
    };
} // base
