#include "PlayerManager.h"
#include "Player.h"

#include <GameWorld.h>

UPlayerManager::UPlayerManager(FContextNode &ctx)
    : IManager(ctx) {
}

UPlayerManager::~UPlayerManager() {
    mPlayerMap.clear();
}

awaitable<void> UPlayerManager::OnPlayerLogin(const std::shared_ptr<UConnection> &conn, const uint64_t pid) {
    if (conn == nullptr || std::any_cast<uint64_t>(conn->GetContext()) != pid) {
        spdlog::error("{} - Null Connection Pointer Or Player ID Not Equal," __FUNCTION__);
        co_return;
    }

    if (const auto plr = FindPlayer(pid); plr != nullptr) {
        spdlog::info("{} - Player[{}] Over Login", __FUNCTION__, pid);

        // 首先断开旧连接
        plr->GetConnection()->ResetContext();
        plr->GetConnection()->Disconnect();

        // 如果已经是登录状态则重新绑定TCP链接即可
        if (plr->IsOnline()) {
            plr->SetConnection(conn);
            plr->OnLogin();

            spdlog::info("{} - Player[{}] Rebind New Connection From {}", __FUNCTION__, pid, conn->RemoteAddress().to_string());
            co_return;
        }

        // 否则直接删掉该对象并执行下面登录逻辑
        RemovePlayer(pid);
        spdlog::info("{} - Remove Old player[{}]", __FUNCTION__, pid);
    }

    const auto plr = EmplacePlayer(conn, pid);
    spdlog::info("{} - New Player[{}] Login", __FUNCTION__, pid);

    co_await plr->OnLogin();
}

void UPlayerManager::OnPlayerLogout(const uint64_t pid) {
    spdlog::info("{} - Player[{}] Logout", __FUNCTION__, pid);
    if (const auto plr = RemovePlayer(pid); plr != nullptr) {
        plr->OnLogout();
    }
}

std::shared_ptr<UPlayer> UPlayerManager::EmplacePlayer(const std::shared_ptr<UConnection> &conn, const uint64_t pid) {
    const auto plr = std::make_shared<UPlayer>(conn);
    PushPlayer(plr);
    return plr;
}

void UPlayerManager::PushPlayer(const std::shared_ptr<UPlayer> &plr) {
    if (!IsSameThread()) {
        RunInThread(&UPlayerManager::PushPlayer, this, plr);
        return;
    }

    std::scoped_lock lock(mMutex);
    mPlayerMap[plr->GetPlayerID()] = plr;
}

std::shared_ptr<UPlayer> UPlayerManager::FindPlayer(const uint64_t pid) {
    std::shared_lock lock(mSharedMutex);
    if (const auto it = mPlayerMap.find(pid); it != mPlayerMap.end()) {
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<UPlayer> UPlayerManager::RemovePlayer(const uint64_t pid) {
    std::scoped_lock lock(mMutex);
    if (const auto it = mPlayerMap.find(pid); it != mPlayerMap.end()) {
        auto res = it->second;
        mPlayerMap.erase(it);
        return res;
    }
    return nullptr;
}

void UPlayerManager::Broadcast(IPackage *pkg, const std::set<uint64_t> &except) {
    if (pkg == nullptr)
        return;

    for (const auto &[pid, plr] : mPlayerMap) {
        if (except.contains(pid))
            continue;

        if (plr->IsOnline()) {
            const auto tPkg = plr->GetConnection()->BuildPackage();
            tPkg->CopyFromOther(pkg);
            plr->Send(tPkg);
        }
    }
    if (const auto pool = pkg->GetOwnerPool(); pool != nullptr) {
        pool->Recycle(pkg);
    }
}

void UPlayerManager::SendToList(IPackage *pkg, const std::set<uint64_t>& players) {
    if (pkg == nullptr)
        return;

    for (const auto pid : players) {
        if (const auto plr = FindPlayer(pid); plr != nullptr && plr->IsOnline()) {
            const auto tPkg = plr->GetConnection()->BuildPackage();
            tPkg->CopyFromOther(pkg);
            plr->Send(tPkg);
        }
    }

    if (const auto pool = pkg->GetOwnerPool(); pool != nullptr) {
        pool->Recycle(pkg);
    }
}
