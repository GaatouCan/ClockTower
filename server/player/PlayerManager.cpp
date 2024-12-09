﻿#include "PlayerManager.h"
#include "Player.h"

#include <game_world.h>
#include <system/config/ConfigSystem.h>

UPlayerManager::UPlayerManager(FContextNode &ctx)
    : IManager(ctx) {
}

UPlayerManager::~UPlayerManager() {
    mPlayerMap.clear();
}

void UPlayerManager::OnDayChange() {
    for (const auto &plr: mPlayerMap | std::views::values) {
        if (plr != nullptr && plr->IsOnline()) {
            plr->OnDayChange();
        }
    }
}

awaitable<std::shared_ptr<UPlayer>> UPlayerManager::OnPlayerLogin(const std::shared_ptr<UConnection> &conn, const FPlayerID &id) {
    if (conn == nullptr || std::any_cast<FPlayerID>(conn->GetContext()) != id) {
        spdlog::error("{} - Null Connection Pointer Or Player ID Not Equal," __FUNCTION__);
        co_return nullptr;
    }

    if (const auto plr = FindPlayer(id.localID); plr != nullptr) {
        {
            std::scoped_lock lock(mPlayerMutex);
            mPlayerMap.erase(id.localID);
        }

        spdlog::info("{} - Player[{}] Over Login", __FUNCTION__, plr->GetFullID());

        plr->TryLeaveScene();

        if (plr->IsOnline()) {
            plr->OnLogout(true, conn->RemoteAddress().to_string());
        }

        plr->GetConnection()->ResetContext();
        plr->GetConnection()->Disconnect();
    }

    const auto plr = std::make_shared<UPlayer>(conn);

    {
        std::scoped_lock lock(mPlayerMutex);
        mPlayerMap[id.localID] = plr;
    }

    spdlog::info("{} - New Player[{}] Login", __FUNCTION__, plr->GetFullID());
    co_await plr->OnLogin();

    // 同步玩家缓存数据
    SyncCache(plr);

    co_return plr;
}

void UPlayerManager::OnPlayerLogout(const FPlayerID pid) {
    spdlog::info("{} - Player[{}] Logout", __FUNCTION__, pid.ToUInt64());
    if (const auto plr = RemovePlayer(pid.localID); plr != nullptr) {
        plr->TryLeaveScene();
        plr->OnLogout();
    }
}

std::shared_ptr<UPlayer> UPlayerManager::FindPlayer(const uint32_t pid) {
    std::shared_lock lock(mPlayerSharedMutex);
    if (const auto it = mPlayerMap.find(pid); it != mPlayerMap.end()) {
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<UPlayer> UPlayerManager::RemovePlayer(const uint32_t pid) {
    std::scoped_lock lock(mPlayerMutex);
    if (const auto it = mPlayerMap.find(pid); it != mPlayerMap.end()) {
        auto res = it->second;
        mPlayerMap.erase(it);
        return res;
    }
    return nullptr;
}

void UPlayerManager::SendToList(IPackage *pkg, const std::set<FPlayerID>& players) {
    if (pkg == nullptr)
        return;

    for (const auto [localID, crossID] : players) {

        if (const auto plr = FindPlayer(localID); plr != nullptr && plr->IsOnline()) {
            const auto tPkg = plr->GetConnection()->BuildPackage();
            tPkg->CopyFromOther(pkg);
            plr->SendPackage(tPkg);
        }
    }

    pkg->Invalid();
    if (const auto pool = pkg->GetOwnerPool(); pool != nullptr) {
        pool->Recycle(pkg);
    }
}

void UPlayerManager::SyncCache(const std::shared_ptr<UPlayer> &plr) {
    if (plr == nullptr)
        return;

    FCacheNode node;
    plr->SyncCache(&node);

    SyncCache(node);
}

void UPlayerManager::SyncCache(const uint32_t pid) {
    if (pid < kPlayerLocalIDBegin || pid > kPlayerLocalIDEnd)
        return;

    const auto plr = FindPlayer(pid);
    SyncCache(plr);
}

void UPlayerManager::SyncCache(const FCacheNode &node) {
    if (!node.pid.IsValid())
        return;

    std::scoped_lock lock(mCacheMutex);
    mCacheMap[node.pid.localID] = node;
    spdlog::info("{} - Player[{}] Success.", __FUNCTION__, node.pid.ToUInt64());
}

awaitable<std::optional<FCacheNode>> UPlayerManager::FindCacheNode(const FPlayerID &pid) {
    if (!pid.IsValid())
        co_return std::nullopt;

    if (pid.crossID != GetServerID()) {
        // TODO
        co_return std::nullopt;
    }

    if (const auto plr = FindPlayer(pid.localID); plr != nullptr) {
        SyncCache(plr);
    }

    std::shared_lock lock(mCacheSharedMutex);
    if (const auto it = mCacheMap.find(pid.localID); it != mCacheMap.end()) {
        co_return std::make_optional(it->second);
    }

    co_return std::nullopt;
}
