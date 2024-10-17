#include "ManagerSystem.h"

#include "../../player/PlayerManager.h"

namespace base {
    void ManagerSystem::LoadManager() {
        REGISTER_MANAGER(PlayerManager)
    }
}
