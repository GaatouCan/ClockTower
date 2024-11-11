#pragma once

#include "../../../system/manager/Manager.h"


class UChatManager final : public IManager {

    MANAGER_BODY(UChatManager)

    explicit UChatManager(asio::io_context &ctx);
    ~UChatManager() override;
};
