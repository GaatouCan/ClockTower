#include "PlayerManager.h"
#include "../../GameWorld.h"


UPlayerManager::UPlayerManager(asio::io_context &ctx)
    : IManager(ctx) {
}

UPlayerManager::~UPlayerManager() {
    mPlayerMap.clear();
}

awaitable<void> UPlayerManager::OnPlayerLogin(const std::shared_ptr<UConnection> &conn, const uint64_t pid) {
    if (const auto plr = FindPlayer(pid); plr != nullptr) {
        spdlog::info("{} - Player[{}] Over Login", __FUNCTION__, pid);

        // 首先断开旧连接
        plr->GetConnection()->ResetContext();
        plr->GetConnection()->Disconnect();

        // 如果已经是登录状态则重新绑定TCP链接即可
        if (plr->IsLogin()) {
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

    // if (const auto sys = GetSystem<UDatabaseSystem>(); sys != nullptr) {
    //     co_await sys->AsyncPushTask([plr](mysqlx::Schema &schema) {
    //     plr->GetComponentModule().Deserialize(schema);
    //     }, asio::use_awaitable);
    // } else {
    //     spdlog::error("{} - Failed To Get Database System.", __FUNCTION__);
    // }

    plr->OnLogin();
}

void UPlayerManager::OnPlayerLogout(const uint64_t pid) {
    spdlog::info("{} - Player[{}] Logout", __FUNCTION__, pid);
    if (const auto plr = RemovePlayer(pid); plr != nullptr) {
        plr->OnLogout();

        // if (const auto sys = GetSystem<UDatabaseSystem>(); sys != nullptr) {
        //     sys->PushTask([plr](mysqlx::Schema &schema) {
        //         plr->GetComponentModule().Serialize(schema);
        //     });
        // } else {
        //     spdlog::error("{} - Failed To Get Database System", __FUNCTION__);
        // }
    }
}

std::shared_ptr<IAbstractPlayer> UPlayerManager::EmplacePlayer(const std::shared_ptr<UConnection> &conn, const uint64_t pid) {
    const auto plr = CreatePlayer(conn, pid);
    PushPlayer(plr);
    return plr;
}

void UPlayerManager::PushPlayer(const std::shared_ptr<IAbstractPlayer>& plr) {
    if (!IsSameThread()) {
        RunInThread(&UPlayerManager::PushPlayer, this, plr);
        return;
    }

    std::scoped_lock lock(mMutex);
    mPlayerMap[plr->GetPlayerId()] = plr;
}

std::shared_ptr<IAbstractPlayer> UPlayerManager::FindPlayer(const uint64_t pid) {
    std::shared_lock lock(mSharedMutex);
    if (const auto it = mPlayerMap.find(pid); it != mPlayerMap.end()) {
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<IAbstractPlayer> UPlayerManager::RemovePlayer(const uint64_t pid) {
    std::scoped_lock lock(mMutex);
    if (const auto it = mPlayerMap.find(pid); it != mPlayerMap.end()) {
        auto res = it->second;
        mPlayerMap.erase(it);
        return res;
    }
    return nullptr;
}
