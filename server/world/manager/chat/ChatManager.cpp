#include "ChatManager.h"
#include "../../../player/Player.h"
#include "../../../common/proto_def.h"


UChatManager::UChatManager(asio::io_context &ctx)
    : IManager(ctx) {
}

UChatManager::~UChatManager() {
}

awaitable<void> protocol::CS_Chat_Room_Request(const std::shared_ptr<ICharacter> &plr, IPackage *pkg) {
    // TODO
    co_return;
}
