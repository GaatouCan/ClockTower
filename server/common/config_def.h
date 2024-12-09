#pragma once

#include <system/config/config_system.h>

#include "../config/AvatarLogicConfig.h"

inline void LoadLogicConfig(UConfigSystem *sys) {
    if (sys == nullptr)
        return;

    REGISTER_LOGIC_CONFIG(UAvatarLogicConfig, "appearance.avatar")
}
