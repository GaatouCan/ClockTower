#include "ChatManager.h"
#include "../../../player/Player.h"
#include "../../../common/proto_def.h"

UChatManager::UChatManager(FContextNode &ctx)
    : IManager(ctx) {
}

UChatManager::~UChatManager() {
}

void protocol::CS_ChatRoomRequest(const std::shared_ptr<IAbstractPlayer> &plr, IPackage *pkg) {
    // TODO
    return;
}
