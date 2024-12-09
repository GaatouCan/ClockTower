#pragma once

#include <system/manager/manager_system.h>

#include "../player/PlayerManager.h"
#include "../command/CommandManager.h"

#include "../world/manager/chat/ChatManager.h"

inline void LoadManager(UManagerSystem *sys) {
    if (sys == nullptr)
        return;

    sys->CreateManager<UPlayerManager>();
    // sys->CreateManager<UCommandManager>();

    // sys->CreateManager<UChatManager>();
}
