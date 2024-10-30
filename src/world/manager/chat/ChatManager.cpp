#include "ChatManager.h"
#include "../Protocol.generated.h"


ChatManager::ChatManager(asio::io_context &ctx)
    : IManager(ctx) {
}

ChatManager::~ChatManager() {
}

awaitable<void> protocol::CS_Chat_Room_Request(const std::shared_ptr<UPlayer> &plr, base::FPackage *pkg) {
    // TODO
    co_return;
}
