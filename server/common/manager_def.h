#pragma once

#include <system/manager/ManagerSystem.h>

#include <manager/player/PlayerManager.h>
#include "../command/CommandManager.h"

#include "../player/PlayerCache.h"
#include "../world/manager/chat/ChatManager.h"

inline void LoadManager(UManagerSystem *sys) {
    if (sys == nullptr)
        return;

    sys->CreateManager<UPlayerManager>();
    sys->CreateManager<UCommandManager>();

    sys->CreateManager<UPlayerCache>();
    sys->CreateManager<UChatManager>();
}
