#include "ChatManager.h"
#include "../../../player/Player.h"
#include "../../../common/proto_def.h"


ChatManager::ChatManager(asio::io_context &ctx)
    : IManager(ctx) {
}

ChatManager::~ChatManager() {
}

awaitable<void> protocol::CS_Chat_Room_Request(const std::shared_ptr<ICharacter> &plr, IPackage *pkg) {
    // TODO
    co_return;
}
