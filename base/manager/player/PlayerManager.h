#pragma once

#include "AbstractPlayer.h"

#include "../../system/manager/Manager.h"

#include <mutex>
#include <shared_mutex>

class UPlayerManager final : public IManager {

    std::map<uint64_t, std::shared_ptr<IAbstractPlayer>> mPlayerMap;
    std::mutex mMutex;
    std::shared_mutex mSharedMutex;

public:
    explicit UPlayerManager(asio::io_context &ctx);
    ~UPlayerManager() override;

    [[nodiscard]] constexpr const char * GetManagerName() const override {
        return "UPlayerManager";
    }

    awaitable<void> OnPlayerLogin(const std::shared_ptr<UConnection> &conn, uint64_t pid);
    void OnPlayerLogout(uint64_t pid);

    std::shared_ptr<IAbstractPlayer> EmplacePlayer(const std::shared_ptr<UConnection> &conn, uint64_t pid);
    void PushPlayer(const std::shared_ptr<IAbstractPlayer>& plr);

    std::shared_ptr<IAbstractPlayer> FindPlayer(uint64_t pid);
    std::shared_ptr<IAbstractPlayer> RemovePlayer(uint64_t pid);
};
