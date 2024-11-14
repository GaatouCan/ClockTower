#include "ChatManager.h"
#include "../../../player/Player.h"
#include "../../../common/proto_def.h"
#include "../system/manager/ManagerSystem.h"


REGISTER_MANAGER(UChatManager)

UChatManager::UChatManager(asio::io_context &ctx)
    : IManager(ctx) {
}

UChatManager::~UChatManager() {
}

awaitable<void> protocol::CS_ChatRoomRequest(const std::shared_ptr<UCharacter> &plr, IPackage *pkg) {
    // TODO
    co_return;
}
