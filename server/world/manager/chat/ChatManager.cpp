#include "ChatManager.h"
#include "../../../player/Player.h"
#include "../../../common/proto_def.h"

UChatManager::UChatManager(asio::io_context &ctx)
    : IManager(ctx) {
}

UChatManager::~UChatManager() {
}

awaitable<void> protocol::CS_ChatRoomRequest(const std::shared_ptr<IAbstractPlayer> &plr, IPackage *pkg) {
    // TODO
    co_return;
}
