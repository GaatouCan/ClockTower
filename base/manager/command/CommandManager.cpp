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

awaitable<void> UCommandManager::OnClientCommand(const std::shared_ptr<IAbstractPlayer> &player, const std::string &type, const std::string &args) {
    const auto iter = mClientCommandMap.find(type);
    if (iter == mClientCommandMap.end()) {
        co_return;
    }

    const UCommandObject obj(type, args);

    if (const auto cmd = std::dynamic_pointer_cast<IClientCommand>(std::invoke(iter->second, obj)); cmd != nullptr) {
        cmd->SetSender(player->GetPlayerID());
        co_await cmd->Execute();
    }
}

awaitable<void> UCommandManager::OnOperateCommand(uint64_t command_id, const std::string &type, const std::string &args) {
    const auto iter = mOperateCommandMap.find(type);
    if (iter == mOperateCommandMap.end()) {
        co_return;
    }

    const UCommandObject obj(type, args);

    if (const auto cmd = std::dynamic_pointer_cast<IOperateCommand>(std::invoke(iter->second, obj)); cmd != nullptr) {
        cmd->SetCommandID(command_id);
        co_await cmd->Execute();
    }
}
