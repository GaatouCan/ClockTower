#pragma once

#include <system/manager/Manager.h>

class UChatManager final : public IManager {

public:

    explicit UChatManager(FContextNode &ctx);
    ~UChatManager() override;

    [[nodiscard]] constexpr const char * GetManagerName() const override {
        return "UChatManager";
    }
};
