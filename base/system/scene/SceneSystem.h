#pragma once

#include "../../SubSystem.h"

class UScene;

class USceneSystem final : public ISubSystem {

public:
    USceneSystem();
    ~USceneSystem() override;

    void Init() override;
    [[nodiscard]] constexpr const char *GetSystemName() const override { return "USceneSystem"; }
};

