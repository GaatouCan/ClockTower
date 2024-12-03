#include "CommandManager.h"

#include "../player/Player.h"

#include <system/database/DatabaseSystem.h>
#include <system/database/Deserializer.h>
#include <GameWorld.h>

#include <spdlog/spdlog.h>
#include <orm/command.orm.h>


UCommandManager::UCommandManager(FContextNode &ctx)
    : IManager(ctx) {

    mClientLogger = spdlog::get("client_command_logger");
    mOperateLogger = spdlog::get("operate_command_logger");

    if (mClientLogger == nullptr || mOperateLogger == nullptr) {
        spdlog::warn("{} - Please Define Logger For Command Manager", __FUNCTION__);
    }
}

UCommandManager::~UCommandManager() {
}

void UCommandManager::OnTick(ATimePoint now) {
    IManager::OnTick(now);

    co_spawn(GetIOContext(), FetchOperateCommand(), detached);
}

awaitable<void> UCommandManager::OnClientCommand(
    const std::shared_ptr<UPlayer> &player,
    const std::string &type,
    const std::string &args)
{
    // const auto iter = mClientCommandMap.find(type);
    // if (iter == mClientCommandMap.end()) {
    //     co_return;
    // }

    const UCommandObject obj(type, args);

    // if (const auto cmd = std::dynamic_pointer_cast<IClientCommand>(std::invoke(iter->second, obj)); cmd != nullptr) {
    //     cmd->SetSender(player->GetFullID());
    //     bool res = co_await cmd->Execute();
    // }
}

awaitable<void> UCommandManager::OnOperateCommand(
    const uint64_t commandID,
    const uint64_t createTime,
    const std::string &creator,
    const std::string &type,
    const std::string &args)
{
    // const auto iter = mOperateCommandMap.find(type);
    // if (iter == mOperateCommandMap.end()) {
    //     co_return;
    // }

    const UCommandObject obj(type, args);

    // if (const auto cmd = std::dynamic_pointer_cast<IOperateCommand>(std::invoke(iter->second, obj)); cmd != nullptr) {
    //     cmd->SetCommandID(commandID);
    //     cmd->SetCreateTime(createTime);
    //     cmd->SetCreator(creator);
    //     bool res = co_await cmd->Execute();
    // }
}

awaitable<void> UCommandManager::FetchOperateCommand() {
    const auto sys = GetSystem<UDatabaseSystem>();
    if (sys == nullptr) {
        spdlog::warn("UCommandManager - Database System Not Found");
        co_return;
    }

    auto res = co_await sys->AsyncSelect("command", "finish_time = 0" ,asio::use_awaitable);
    if (!res.has_value())
        co_return;

    UDeserializer deserializer((std::move(*res)));

    while (deserializer.HasMore()) {
        auto row = deserializer.TDeserialize<orm::UDBTable_Command>();
        co_await OnOperateCommand(row.id, row.create_time, row.creator, row.type, row.param);
    }
    // for (auto row : res->fetchAll()) {
    //     const uint64_t id = row[0].get<uint64_t>();
    //     auto creator = row[1].get<std::string>();
    //     auto type = row[2].get<std::string>();
    //     auto param = row[3].get<std::string>();
    //     const uint64_t create_time = row[4].get<uint64_t>();
    //     const uint64_t finish_time = row[5].get<uint64_t>();
    //     auto extend = row[6].get<std::string>();
    //
    //     co_await OnOperateCommand(id, create_time, creator, type, param);
    //     sys->PushTask([id](mysqlx::Schema &schema) -> bool {
    //         if (auto table = schema.getTable("command"); table.existsInDatabase()) {
    //             table.update().set("finish_time", 1).where("id = : id").bind("id", id).execute();
    //             return true;
    //         }
    //         return false;
    //     });
    // }
}
