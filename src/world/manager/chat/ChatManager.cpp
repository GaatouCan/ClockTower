#include "ChatManager.h"
#include "../Protocol.generated.h"


ChatManager::ChatManager(asio::io_context &ctx)
    : IManager(ctx) {
}

ChatManager::~ChatManager() {
}

awaitable<void> protocol::CS_Chat_Room_Request(const std::shared_ptr<Player> &plr, base::Package *pkg) {
    // TODO
    co_return;
}
