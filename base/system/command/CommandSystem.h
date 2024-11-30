#pragma once

#include "../../SubSystem.h"

class UCommandSystem final : public ISubSystem {
public:
    UCommandSystem();
    ~UCommandSystem() override;

    void Init() override;

    [[nodiscard]] constexpr const char *GetSystemName() const override { return "UCommandSystem"; }
};
