#pragma once

#include "../../system/manager/Manager.h"


class UCommandManager final : public IManager {

public:
    explicit UCommandManager(asio::io_context &ctx);
    ~UCommandManager() override;

    [[nodiscard]] constexpr const char * GetManagerName() const override {
        return "UCommandManager";
    }
};
