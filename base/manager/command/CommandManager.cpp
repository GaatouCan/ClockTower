#include "CommandManager.h"
#include "CommandObject.h"

#include "../player/AbstractPlayer.h"

UCommandManager::UCommandManager(asio::io_context &ctx)
    : IManager(ctx) {
}

UCommandManager::~UCommandManager() {
}

void UCommandManager::OnClientCommand(const std::shared_ptr<IAbstractPlayer> &player, const std::string &type, const std::string &args) {
    const auto iter = mClientCmdMap.find(type);
    if (iter == mClientCmdMap.end()) {
        return;
    }

    const UCommandObject obj(type, args);

    if (const auto cmd = dynamic_cast<IClientCommand*>(std::invoke(iter->second, obj)); cmd != nullptr) {
        cmd->SetSender(player->GetPlayerID());
        cmd->Execute();

        delete cmd;
    }
}
