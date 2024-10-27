#include "ManagerSystem.h"

#include "../../player/PlayerManager.h"
#include "../../player/PlayerCache.h"
#include "../../world/manager/chat/ChatManager.h"

namespace base {
    void ManagerSystem::LoadManager() {
        REGISTER_MANAGER(PlayerManager)
        REGISTER_MANAGER(PlayerCache)
        REGISTER_MANAGER(ChatManager)
    }
}
