#pragma once

#include "AbstractPlayer.h"

#include "../../system/manager/Manager.h"

#include <mutex>
#include <shared_mutex>

class UPlayerManager final : public IManager {

    static std::function<std::shared_ptr<IAbstractPlayer>(const AConnectionPointer&, uint64_t)> sPlayerCreator;

    std::map<uint64_t, std::shared_ptr<IAbstractPlayer>> mPlayerMap;
    std::mutex mMutex;
    std::shared_mutex mSharedMutex;

public:
    explicit UPlayerManager(asio::io_context &ctx);
    ~UPlayerManager() override;

    [[nodiscard]] constexpr const char * GetManagerName() const override {
        return "UPlayerManager";
    }

    static void SetPlayerCreator(const std::function<std::shared_ptr<IAbstractPlayer>(const AConnectionPointer&, uint64_t)> &func);

    void OnPlayerLogin(const std::shared_ptr<UConnection> &conn, uint64_t pid);
    void OnPlayerLogout(uint64_t pid);

    std::shared_ptr<IAbstractPlayer> EmplacePlayer(const std::shared_ptr<UConnection> &conn, uint64_t pid);
    void PushPlayer(const std::shared_ptr<IAbstractPlayer>& plr);

    std::shared_ptr<IAbstractPlayer> FindPlayer(uint64_t pid);
    std::shared_ptr<IAbstractPlayer> RemovePlayer(uint64_t pid);

    void Broadcast(IPackage *pkg, uint64_t expect = 0);
};
