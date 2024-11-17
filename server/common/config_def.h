#pragma once

#include <system/config/ConfigSystem.h>

#include "../config/AvatarLogicConfig.h"

inline void LoadLogicConfig(UConfigSystem *sys) {
    if (sys == nullptr)
        return;

    sys->CreateLogicConfig<UAvatarLogicConfig>({"appearance.avatar"});
}
