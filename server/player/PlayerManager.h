#pragma once

#include "CacheNode.h"

#include <system/manager/Manager.h>
#include <PlayerID.h>

#include <mutex>
#include <shared_mutex>


class UPlayer;
class UConnection;

class UPlayerManager final : public IManager {

    std::map<FPlayerID, std::shared_ptr<UPlayer>> mPlayerMap;
    std::mutex mPlayerMutex;
    mutable std::shared_mutex mPlayerSharedMutex;

    std::map<FPlayerID, FCacheNode> mCacheMap;
    std::mutex mCacheMutex;
    mutable std::shared_mutex mCacheSharedMutex;

public:
    explicit UPlayerManager(FContextNode &ctx);
    ~UPlayerManager() override;

    [[nodiscard]] constexpr const char * GetManagerName() const override {
        return "UPlayerManager";
    }

    awaitable<std::shared_ptr<UPlayer>> OnPlayerLogin(const std::shared_ptr<UConnection> &conn, const FPlayerID &id);

    void OnPlayerLogout(FPlayerID pid);

    std::shared_ptr<UPlayer> FindPlayer(const FPlayerID &pid);
    std::shared_ptr<UPlayer> RemovePlayer(const FPlayerID &pid);

    // void Broadcast(IPackage *pkg, const std::set<uint64_t> &except = {});

    void SendToList(IPackage *pkg, const std::set<FPlayerID>& players);

    void SyncCache(const std::shared_ptr<UPlayer> &plr);
    void SyncCache(const FPlayerID &pid);
    void SyncCache(const FCacheNode &node);

    awaitable<std::optional<FCacheNode>> FindCacheNode(const FPlayerID &pid);
};
