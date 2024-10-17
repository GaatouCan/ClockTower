#include "PlayerManager.h"

PlayerManager::PlayerManager(asio::io_context &ctx)
    : IManager(ctx) {
}

PlayerManager::~PlayerManager() {
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
