#pragma once

#include "../../sub_system.h"

class UScene;

class USceneSystem final : public ISubSystem {

    std::vector<UScene *> mSceneVector;

public:
    ~USceneSystem() override;

    void Init() override;
    [[nodiscard]] constexpr const char *GetSystemName() const override { return "USceneSystem"; }

    [[nodiscard]] UScene *GetMainScene() const;
    [[nodiscard]] UScene *GetSceneByID(uint32_t id) const;
};

