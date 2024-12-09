#pragma once

#include "ChatRoom.h"

#include <unique_id.h>
#include <system/manager/manager.h>


class UChatManager final : public IManager {

    std::map<FUniqueID, UChatRoom *> mChatRoomMap;

public:

    explicit UChatManager(FContextNode &ctx);
    ~UChatManager() override;

    [[nodiscard]] constexpr const char * GetManagerName() const override {
        return "UChatManager";
    }
};
