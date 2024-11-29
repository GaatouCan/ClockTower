#include "CommandManager.h"
#include "CommandObject.h"

#include "../player/AbstractPlayer.h"
#include "../../system/database/DatabaseSystem.h"
#include "../../GameWorld.h"

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

    mClientLogger = spdlog::get("client_command_logger");
    mOperateLogger = spdlog::get("operate_command_logger");

    if (mClientLogger == nullptr || mOperateLogger == nullptr) {
        spdlog::warn("{} - Please Define Logger For Command Manager", __FUNCTION__);
    }

    // const auto sys = GetSystem<UDatabaseSystem>();
    // if (sys == nullptr) {
    //     spdlog::warn("UCommandManager - Database System Not Found");
    //     return;
    // }
    //
    // sys->SyncSelect("command", "update_time == 0", [this](mysqlx::Row row) {
    //     // TODO
    // });
}

UCommandManager::~UCommandManager() {
}

void UCommandManager::SetClientCommandRegister(const std::function<void(UCommandManager *)> &func) {
    sClientCommandRegister = func;
}

void UCommandManager::SetOperateCommandRegister(const std::function<void(UCommandManager *)> &func) {
    sOperateCommandRegister = func;
}

void UCommandManager::OnTick(ATimePoint now) {
    IManager::OnTick(now);

    co_spawn(GetIOContext(), FetchOperateCommand(), detached);
}

awaitable<void> UCommandManager::OnClientCommand(
    const std::shared_ptr<IAbstractPlayer> &player,
    const std::string &type,
    const std::string &args)
{
    const auto iter = mClientCommandMap.find(type);
    if (iter == mClientCommandMap.end()) {
        co_return;
    }

    const UCommandObject obj(type, args);

    if (const auto cmd = std::dynamic_pointer_cast<IClientCommand>(std::invoke(iter->second, obj)); cmd != nullptr) {
        cmd->SetSender(player->GetPlayerID());
        bool res = co_await cmd->Execute();
    }
}

awaitable<void> UCommandManager::OnOperateCommand(
    const uint64_t commandID,
    const uint64_t createTime,
    const std::string &creator,
    const std::string &type,
    const std::string &args)
{
    const auto iter = mOperateCommandMap.find(type);
    if (iter == mOperateCommandMap.end()) {
        co_return;
    }

    const UCommandObject obj(type, args);

    if (const auto cmd = std::dynamic_pointer_cast<IOperateCommand>(std::invoke(iter->second, obj)); cmd != nullptr) {
        cmd->SetCommandID(commandID);
        cmd->SetCreateTime(createTime);
        cmd->SetCreator(creator);
        bool res = co_await cmd->Execute();
    }
}

awaitable<void> UCommandManager::FetchOperateCommand() {
    const auto sys = GetSystem<UDatabaseSystem>();
    if (sys == nullptr) {
        spdlog::warn("UCommandManager - Database System Not Found");
        co_return;
    }

    auto res = co_await sys->AsyncPushTask([](mysqlx::Schema &schema) -> mysqlx::RowResult {
        mysqlx::Table table = schema.getTable("command");
        if (!table.existsInDatabase())
            return {};

        return table.select().where("update_time = 0").execute();
    }, asio::use_awaitable);
}
