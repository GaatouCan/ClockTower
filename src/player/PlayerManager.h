#pragma once

#include "../system/manager/IManager.h"
#include "Player.h"

#include <unordered_map>

class PlayerManager final : public base::IManager {

    std::unordered_map<uint64_t, std::shared_ptr<Player>> playerMap_;

public:
    explicit PlayerManager(asio::io_context &ctx);
    ~PlayerManager() override;

    /**
     * Call in the same thread with connection
     */
    std::shared_ptr<Player> EmplacePlayer(const std::shared_ptr<base::Connection> &conn, uint64_t pid);
    void PushPlayer(const std::shared_ptr<Player>& plr);

    std::shared_ptr<Player> FindPlayer(uint64_t pid);
};
