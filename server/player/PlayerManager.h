#pragma once

#include <system/manager/Manager.h>

#include <mutex>
#include <shared_mutex>

class UPlayer;
class UConnection;

class UPlayerManager final : public IManager {

    std::map<uint64_t, std::shared_ptr<UPlayer>> mPlayerMap;
    std::mutex mMutex;
    std::shared_mutex mSharedMutex;

public:
    explicit UPlayerManager(FContextNode &ctx);
    ~UPlayerManager() override;

    [[nodiscard]] constexpr const char * GetManagerName() const override {
        return "UPlayerManager";
    }

    awaitable<void> OnPlayerLogin(const std::shared_ptr<UConnection> &conn, uint64_t pid);
    void OnPlayerLogout(uint64_t pid);

    std::shared_ptr<UPlayer> EmplacePlayer(const std::shared_ptr<UConnection> &conn, uint64_t pid);
    void PushPlayer(const std::shared_ptr<UPlayer>& plr);

    std::shared_ptr<UPlayer> FindPlayer(uint64_t pid);
    std::shared_ptr<UPlayer> RemovePlayer(uint64_t pid);

    void Broadcast(IPackage *pkg, const std::set<uint64_t> &except = {});

    void SendToList(IPackage *pkg, const std::set<uint64_t>& players);
};
