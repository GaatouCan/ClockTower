#pragma once

#include "../../../system/manager/Manager.h"


class ChatManager final : public IManager {

public:
    explicit ChatManager(asio::io_context &ctx);
    ~ChatManager() override;
};
