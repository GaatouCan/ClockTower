#include "ManagerSystem.h"

#include "../../player/PlayerManager.h"
#include "../../player/PlayerCache.h"

namespace base {
    void ManagerSystem::LoadManager() {
        REGISTER_MANAGER(PlayerManager)
        REGISTER_MANAGER(PlayerCache)
    }
}
