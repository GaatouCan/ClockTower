#include "PlayerManager.h"
#include "../base/GameWorld.h"
#include "../system/database/DatabaseSystem.h"

PlayerManager::PlayerManager(asio::io_context &ctx)
    : IManager(ctx) {
}

PlayerManager::~PlayerManager() {
    playerMap_.clear();
}

void PlayerManager::Init() {

}

awaitable<void> PlayerManager::OnPlayerLogin(const std::shared_ptr<base::Connection> &conn, const uint64_t pid) {
    if (const auto old = RemovePlayer(pid); old != nullptr) {
        old->GetConnection()->ResetContext();
        old->GetConnection()->Disconnect();
        old->OnLogout();
    }

    const auto plr = EmplacePlayer(conn, pid);

    const auto sys = GetSystem<base::DatabaseSystem>();
    if (sys == nullptr) {
        spdlog::error("{} - Failed to get DatabaseSystem", __func__);
        co_return;
    }

    sys->PushTask([plr](mysqlx::Schema &schema) {
        plr->GetComponentModule().Deserialize(schema);
        plr->OnLogin();
    });
}

void PlayerManager::OnPlayerLogout(const uint64_t pid) {
    if (const auto plr = RemovePlayer(pid); plr != nullptr) {
        const auto sys = GetSystem<base::DatabaseSystem>();
        if (sys == nullptr) {
            spdlog::error("{} - Failed to get DatabaseSystem", __func__);
            return;
        }

        sys->PushTask([plr](mysqlx::Schema &schema) {
            plr->GetComponentModule().Serialize(schema);
            plr->OnLogout();
        });
    }
}

std::shared_ptr<Player> PlayerManager::EmplacePlayer(const std::shared_ptr<base::Connection> &conn, const uint64_t pid) {
    const auto plr = CreatePlayer(conn, pid);
    PushPlayer(plr);
    return plr;
}

void PlayerManager::PushPlayer(const std::shared_ptr<Player>& plr) {
    if (!IsSameThread()) {
        RunInThread(&PlayerManager::PushPlayer, this, plr);
        return;
    }

    std::unique_lock lock(blockMutex_);
    playerMap_[plr->GetPlayerID()] = plr;
}

std::shared_ptr<Player> PlayerManager::FindPlayer(const uint64_t pid) {
    std::shared_lock lock(sharedMutex_);
    if (const auto it = playerMap_.find(pid); it != playerMap_.end()) {
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<Player> PlayerManager::RemovePlayer(const uint64_t pid) {
    std::unique_lock lock(blockMutex_);
    if (const auto it = playerMap_.find(pid); it != playerMap_.end()) {
        auto res = it->second;
        playerMap_.erase(it);
        return res;
    }
    return nullptr;
}
