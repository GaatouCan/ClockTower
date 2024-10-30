#pragma once

#include "../system/manager/Manager.h"
#include "Player.h"

#include <unordered_map>
#include <mutex>
#include <shared_mutex>

class PlayerManager final : public IManager {

    std::unordered_map<uint64_t, std::shared_ptr<UPlayer>> playerMap_;
    std::mutex mutex_;
    std::shared_mutex sharedMutex_;

public:
    explicit PlayerManager(asio::io_context &ctx);
    ~PlayerManager() override;

    awaitable<void> OnPlayerLogin(const std::shared_ptr<UConnection> &conn, uint64_t pid);
    void OnPlayerLogout(uint64_t pid);

    std::shared_ptr<UPlayer> EmplacePlayer(const std::shared_ptr<UConnection> &conn, uint64_t pid);
    void PushPlayer(const std::shared_ptr<UPlayer>& plr);

    std::shared_ptr<UPlayer> FindPlayer(uint64_t pid);
    std::shared_ptr<UPlayer> RemovePlayer(uint64_t pid);
};
