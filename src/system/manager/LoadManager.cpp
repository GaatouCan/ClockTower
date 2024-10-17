#include "ManagerSystem.h"
#include "../../base/GameWorld.h"

#include "../../player/PlayerManager.h"

namespace base {
    void ManagerSystem::LoadManager() {
        REGISTER_MANAGER(PlayerManager)
    }

    ThreadID ManagerSystem::GetThreadID() const {
        return tid_;
    }

    bool ManagerSystem::InManagerThread() const {
        return tid_ == std::this_thread::get_id();
    }
}
