#pragma once

#include "../../base/ISubSystem.h"

namespace base {
    class ProtocolSystem final : public ISubSystem {

        SUB_SYSTEM_BODY(ProtocolSystem)

        void init() override;

    public:

    };
} // base
