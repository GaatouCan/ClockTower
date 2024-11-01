#pragma once

#include "../system/manager/ManagerSystem.h"

#include "../player/PlayerManager.h"
#include "../player/PlayerCache.h"

inline void LoadManager(UManagerSystem * sys) {

    REGISTER_MANAGER(UPlayerManager)
    REGISTER_MANAGER(UPlayerCache)
}
