#pragma once

#include "ChatRoom.h"

#include <system/manager/Manager.h>


class UChatManager final : public IManager {

    std::map<uint64_t, UChatRoom *> mChatRoomMap;

public:

    explicit UChatManager(FContextNode &ctx);
    ~UChatManager() override;

    [[nodiscard]] constexpr const char * GetManagerName() const override {
        return "UChatManager";
    }
};
