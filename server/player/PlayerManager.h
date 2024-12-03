#pragma once

#include <system/manager/Manager.h>
#include <PlayerID.h>

#include <mutex>
#include <shared_mutex>


class UPlayer;
class UConnection;

class UPlayerManager final : public IManager {

    std::map<FPlayerID, std::shared_ptr<UPlayer>> mPlayerMap;
    std::mutex mMutex;
    std::shared_mutex mSharedMutex;

public:
    explicit UPlayerManager(FContextNode &ctx);
    ~UPlayerManager() override;

    [[nodiscard]] constexpr const char * GetManagerName() const override {
        return "UPlayerManager";
    }



    awaitable<void> OnPlayerLogin(const std::shared_ptr<UPlayer> &plr);
    void OnPlayerLogout(FPlayerID pid);

    // std::shared_ptr<UPlayer> EmplacePlayer(const std::shared_ptr<UConnection> &conn, uint64_t pid);
    // void PushPlayer(const std::shared_ptr<UPlayer>& plr);

    std::shared_ptr<UPlayer> FindPlayer(const FPlayerID &pid);
    std::shared_ptr<UPlayer> RemovePlayer(const FPlayerID &pid);

    // void Broadcast(IPackage *pkg, const std::set<uint64_t> &except = {});
    //
    // void SendToList(IPackage *pkg, const std::set<uint64_t>& players);
};
