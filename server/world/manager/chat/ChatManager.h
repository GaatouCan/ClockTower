#pragma once

#include "../../../system/manager/Manager.h"


class UChatManager final : public IManager {

public:
    explicit UChatManager(asio::io_context &ctx);
    ~UChatManager() override;
};
