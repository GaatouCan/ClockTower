#include "ChatManager.h"
#include "../../../player/Player.h"
#include "../../../common/proto_def.h"

UChatManager::UChatManager(FContextNode &ctx)
    : IManager(ctx) {
}

UChatManager::~UChatManager() {
}

awaitable<void> protocol::CS_ChatRoomRequest(const std::shared_ptr<UCharacter> &plr, IPackage *pkg) {
    // TODO
    co_return;
}

awaitable<void> protocol::CS_ChatToRoomRequest(const std::shared_ptr<UCharacter> &plr, IPackage *pkg) {
    co_return;
}
