#pragma once

#include "../system/manager/IManager.h"

class PlayerManager final : public base::IManager {
public:
    explicit PlayerManager(asio::io_context &ctx);
    ~PlayerManager() override;
};
