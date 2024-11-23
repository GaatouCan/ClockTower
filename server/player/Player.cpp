#include "Player.h"

#include "../common/ProtoType.h"

#include <GameWorld.h>
#include <impl/Package.h>
#include <system/event/EventSystem.h>

#include <utility>
#include <ranges>
#include <login.pb.h>


UPlayer::UPlayer(AConnectionPointer conn)
    : IAbstractPlayer(std::move(conn)),
      mComponentModule(this),
      mEventModule(this) {
}

UPlayer::~UPlayer() {
    for (auto &timer : std::views::values(mTimerMap)) {
        timer.Stop();
    }
}

AThreadID UPlayer::GetThreadID() const {
    return GetConnection()->GetThreadID();
}

bool UPlayer::IsSameThread() const {
    return std::this_thread::get_id() == GetThreadID();
}

UComponentModule &UPlayer::GetComponentModule() {
    return mComponentModule;
}

UEventModule &UPlayer::GetEventModule() {
    return mEventModule;
}

void UPlayer::OnLogin() {
    if (!IsSameThread()) {
        RunInThread(&UPlayer::OnLogin, this);
        return;
    }
    IAbstractPlayer::OnLogin();
    spdlog::info("{} - Player[{}] Login Successfully.", __FUNCTION__, GetPlayerID());

    mLoginTime = std::chrono::steady_clock::now();
    mComponentModule.OnLogin();

    Login::SC_LoginResponse response;
    response.set_result(true);
    response.set_progress(100);
    response.set_describe("Component Load Completed");

    SEND_PACKAGE(this, SC_LoginResponse, response);

    const auto param = new FEP_PlayerLogin;
    param->pid = GetPlayerID();

    DISPATCH_EVENT(PLAYER_LOGIN, param);
}

void UPlayer::OnLogout() {
    if (!IsSameThread()) {
        RunInThread(&UPlayer::OnLogin, this);
        return;
    }
    IAbstractPlayer::OnLogout();
    mTimerMap.clear();

    mComponentModule.OnLogout();
    spdlog::info("{} - Player[{}] Logout.", __FUNCTION__, GetPlayerID());

    const auto param = new FEP_PlayerLogout;
    param->pid = GetPlayerID();

    DISPATCH_EVENT(PLAYER_LOGOUT, param);
}

void UPlayer::StopTimer(const uint64_t timerID) {
    if (const auto iter = mTimerMap.find(timerID); iter != mTimerMap.end()) {
        iter->second.Stop();
        mTimerMap.erase(iter);
    }
}

void UPlayer::Send(const uint32_t id, const std::string_view data) const {
    const auto pkg = dynamic_cast<FPackage *>(BuildPackage());
    pkg->SetPackageID(id).SetData(data);
    IAbstractPlayer::Send(pkg);
}

void UPlayer::Send(const uint32_t id, const std::stringstream &ss) const {
    const auto pkg = dynamic_cast<FPackage *>(BuildPackage());
    pkg->SetPackageID(id).SetData(ss.str());
    IAbstractPlayer::Send(pkg);
}

void UPlayer::SyncCache(FCacheNode *node) {
    mComponentModule.SyncCache(node);
}

std::shared_ptr<UPlayer> CreatePlayer(const AConnectionPointer &conn, const uint64_t pid) {
    auto plr = std::make_shared<UPlayer>(conn);
    // plr->SetPlayerId(pid);

    return plr;
}
