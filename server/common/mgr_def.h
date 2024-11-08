#pragma once

#include "../base/system/manager/ManagerSystem.h"

#include "../player/PlayerManager.h"
#include "../player/PlayerCache.h"
#include "../world/manager/chat/ChatManager.h"

inline void LoadManager(UManagerSystem * sys) {
    assert(sys->IsTimeToLoadManager(), "Manager System is not time-to-load managers");

    REGISTER_MANAGER(UPlayerManager)
    REGISTER_MANAGER(UPlayerCache)
    REGISTER_MANAGER(UChatManager)
}
