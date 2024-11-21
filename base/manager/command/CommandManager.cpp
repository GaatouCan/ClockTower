#include "CommandManager.h"
#include "CommandObject.h"

#include "../player/AbstractPlayer.h"

#include <spdlog/spdlog.h>

std::function<void(UCommandManager*)> UCommandManager::sClientCommandRegister = nullptr;
std::function<void(UCommandManager*)> UCommandManager::sOperateCommandRegister = nullptr;

UCommandManager::UCommandManager(FContextNode &ctx)
    : IManager(ctx) {

    if (sOperateCommandRegister) {
        std::invoke(sOperateCommandRegister, this);
        spdlog::info("UCommandManager - Operate Command Registered");
    }

    if (sClientCommandRegister) {
        std::invoke(sClientCommandRegister, this);
        spdlog::info("UCommandManager - Client Command Registered");
    }
}

UCommandManager::~UCommandManager() {
}

void UCommandManager::SetClientCommandRegister(const std::function<void(UCommandManager *)> &func) {
    sClientCommandRegister = func;
}

void UCommandManager::SetOperateCommandRegister(const std::function<void(UCommandManager *)> &func) {
    sOperateCommandRegister = func;
}

void UCommandManager::OnClientCommand(const std::shared_ptr<IAbstractPlayer> &player, const std::string &type, const std::string &args) {
    const auto iter = mClientCommandMap.find(type);
    if (iter == mClientCommandMap.end()) {
        return;
    }

    const UCommandObject obj(type, args);

    if (const auto cmd = dynamic_cast<IClientCommand*>(std::invoke(iter->second, obj)); cmd != nullptr) {
        cmd->SetSender(player->GetPlayerID());
        cmd->Execute();

        delete cmd;
    }
}
