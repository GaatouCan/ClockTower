#pragma once

#include "../system/manager/IManager.h"

class PlayerCache final : public base::IManager {

public:
    explicit PlayerCache(asio::io_context &ctx);
    ~PlayerCache() override;
};
