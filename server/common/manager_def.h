#pragma once

#include "../system/manager/ManagerSystem.h"

#include "../player/PlayerManager.h"
#include "../player/PlayerCache.h"
#include "../world/manager/chat/ChatManager.h"

inline void LoadManager(UManagerSystem *sys) {
    assert(sys);

    sys->CreateManager<UPlayerManager>();
    sys->CreateManager<UPlayerCache>();
    sys->CreateManager<UChatManager>();
}
