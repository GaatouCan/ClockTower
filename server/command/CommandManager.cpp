#include "CommandManager.h"

#include "../player/Player.h"

#include <system/command/AbstractCommand.h>
#include <system/command/CommandSystem.h>
#include <system/database/DatabaseSystem.h>
#include <system/database/Deserializer.h>
#include <GameWorld.h>

#include <spdlog/spdlog.h>
#include <orm/command.orm.h>

#include "system/database/Serializer.h"


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

    const auto diff = std::chrono::duration_cast<std::chrono::seconds>(now - mFetchTime);
    if (diff.count() < 5)
        return;

    mFetchTime = now;
    co_spawn(GetIOContext(), FetchOperateCommand(), detached);
}

awaitable<void> UCommandManager::OnClientCommand(
    const std::shared_ptr<UPlayer> &player,
    const std::string &type,
    const std::string &args)
{
    const auto sys = GetSystem<UCommandSystem>();
    if (sys == nullptr)
        co_return;

    const auto cmd = sys->CreateClientCommand(type, args);
    if (cmd == nullptr)
        co_return;

    cmd->SetSender(player->GetFullID());
    co_await cmd->Execute();
    co_return;
}

awaitable<void> UCommandManager::OnOperateCommand(
    const uint64_t commandID,
    const uint64_t createTime,
    const std::string &creator,
    const std::string &type,
    const std::string &args)
{
    if (mCurrentOperateCommandSet.contains(commandID))
        co_return;

    orm::UDBTable_Command cmdRow(commandID, creator, type, args, createTime, 1, "");

    const auto dbSys = GetSystem<UDatabaseSystem>();
    if (dbSys == nullptr)
        co_return;

    const bool ret = co_await dbSys->AsyncTask([cmdRow](mysqlx::Schema &schema) mutable -> bool {
        if (auto table = schema.getTable(cmdRow.GetTableName()); table.existsInDatabase()) {
            USerializer serializer(table);
            serializer.Serialize(&cmdRow);
            return true;
        }
        return false;
    }, asio::use_awaitable);

    if (!ret)
        co_return;

    const auto cmdSys = GetSystem<UCommandSystem>();
    if (cmdSys == nullptr)
        co_return;

    const auto cmd = cmdSys->CreateOperateCommand(type, args);
    if (cmd == nullptr)
        co_return;

    cmd->SetCommandID(commandID);
    cmd->SetCreator(creator);
    cmd->SetCreateTime(createTime);

    mCurrentOperateCommandSet.emplace(commandID);

    const bool bFinished = co_await cmd->Execute();
    mCurrentOperateCommandSet.erase(commandID);

    cmdRow.finish_time = bFinished ? UnixTime() : 0;

    co_await dbSys->AsyncTask([cmdRow](mysqlx::Schema &schema) mutable -> bool {
        if (auto table = schema.getTable(cmdRow.GetTableName()); table.existsInDatabase()) {
            USerializer serializer(table);
            serializer.Serialize(&cmdRow);
            return true;
        }
        return false;
    }, asio::use_awaitable);

    co_return;
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
}
