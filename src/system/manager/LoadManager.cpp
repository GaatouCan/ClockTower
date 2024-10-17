#include "ManagerSystem.h"
#include "../../base/GameWorld.h"

#include "../../player/PlayerManager.h"

namespace base {
    void ManagerSystem::LoadManager() {
        REGISTER_MANAGER(PlayerManager)
    }

}