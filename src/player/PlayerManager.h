#pragma once

#include "../system/manager/IManager.h"
#include "Player.h"

#include <unordered_map>
#include <mutex>
#include <shared_mutex>

class PlayerManager final : public base::IManager {

    std::unordered_map<uint64_t, std::shared_ptr<Player>> playerMap_;
    std::mutex blockMutex_;
    std::shared_mutex sharedMutex_;

public:
    explicit PlayerManager(asio::io_context &ctx);
    ~PlayerManager() override;

    void OnPlayerLogin(const std::shared_ptr<base::Connection> &conn, uint64_t pid);
    void OnPlayerLogout(uint64_t pid);

    std::shared_ptr<Player> EmplacePlayer(const std::shared_ptr<base::Connection> &conn, uint64_t pid);
    void PushPlayer(const std::shared_ptr<Player>& plr);

    std::shared_ptr<Player> FindPlayer(uint64_t pid);
    std::shared_ptr<Player> RemovePlayer(uint64_t pid);
};
