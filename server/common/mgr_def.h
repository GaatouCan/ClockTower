#pragma once

#include "../base/system/manager/ManagerSystem.h"

#include "../player/PlayerManager.h"
#include "../player/PlayerCache.h"
// #include "../world/manager/chat/ChatManager.h"

inline void LoadManager(UManagerSystem * sys) {
    assert(sys->IsTimeToLoadManager());

    REGISTER_MANAGER(UPlayerManager)
    REGISTER_MANAGER(UPlayerCache)
    // REGISTER_MANAGER(UChatManager)
}
