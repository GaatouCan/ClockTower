#pragma once

#include "../../../system/manager/IManager.h"


class ChatManager final : public base::IManager {

public:
    explicit ChatManager(asio::io_context &ctx);
    ~ChatManager() override;
};
