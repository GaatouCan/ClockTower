#pragma once

#include "../system/manager/ManagerSystem.h"

#include "../player/PlayerManager.h"
#include "../player/PlayerCache.h"

inline void LoadManager(UManagerSystem * sys) {
    if (sys == nullptr)
        return;

    sys->CreateManager<UPlayerManager>();
    sys->CreateManager<UPlayerCache>();

}
