#include "Player.h"

#include "../common/ProtoType.h"

#include <GameWorld.h>
#include <impl/Package.h>
#include <system/event/EventSystem.h>

#include <utility>
#include <login.pb.h>


UPlayer::UPlayer(AConnectionPointer conn)
    : mConn(std::move(conn)),
      mId(std::any_cast<uint64_t>(mConn->GetContext())),
      mComponentModule(this),
      mEventModule(this) {
}

UPlayer::~UPlayer() {
}

UPlayer &UPlayer::SetPlayerId(const uint64_t id) {
    mId = id;
    return *this;
}

uint64_t UPlayer::GetPlayerID() const {
    return mId;
}

AConnectionPointer UPlayer::GetConnection() const {
    return mConn;
}

void UPlayer::SetConnection(const AConnectionPointer &conn) {
    mConn = conn;
}

AThreadID UPlayer::GetThreadID() const {
    return mConn->GetThreadID();
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
    spdlog::info("{} - Player[{}] Login Successfully.", __FUNCTION__, GetPlayerID());

    mLoginTime = std::chrono::steady_clock::now();
    mComponentModule.OnLogin();

    Login::SC_LoginResponse response;
    response.set_result(true);
    response.set_progress(100);
    response.set_describe("Component Load Completed");

    SendPackage(SC_LoginResponse, response);

    const auto param = new FEP_PlayerLogin;
    param->pid = mId;

    DispatchEvent(PLAYER_LOGIN, param);
}

void UPlayer::OnLogout() {
    if (!IsSameThread()) {
        RunInThread(&UPlayer::OnLogin, this);
        return;
    }

    mTimerMap.clear();

    mComponentModule.OnLogout();
    spdlog::info("{} - Player[{}] Logout.", __FUNCTION__, GetPlayerID());

    const auto param = new FEP_PlayerLogout;
    param->pid = mId;
    DispatchEvent(PLAYER_LOGOUT, param);
}

bool UPlayer::IsLogin() const {
    return mLoginTime.time_since_epoch().count() > 0;
}

void UPlayer::StopTimer(const uint64_t timerID) {
    if (const auto iter = mTimerMap.find(timerID); iter != mTimerMap.end()) {
        iter->second.Stop();
        mTimerMap.erase(iter);
    }
}

IPackage *UPlayer::BuildPackage() const {
    return mConn->BuildPackage();
}

void UPlayer::Send(IPackage *pkg) const {
    mConn->Send(pkg);
}

void UPlayer::Send(const uint32_t id, const std::string_view data) const {
    const auto pkg = dynamic_cast<FPackage *>(mConn->BuildPackage());
    pkg->SetPackageID(id).SetData(data);
    Send(pkg);
}

void UPlayer::Send(const uint32_t id, const std::stringstream &ss) const {
    const auto pkg = dynamic_cast<FPackage *>(mConn->BuildPackage());
    pkg->SetPackageID(id).SetData(ss.str());
    Send(pkg);
}

void UPlayer::SyncCache(FCacheNode *node) {
    mComponentModule.SyncCache(node);
}

std::shared_ptr<UPlayer> CreatePlayer(const AConnectionPointer &conn, const uint64_t pid) {
    auto plr = std::make_shared<UPlayer>(conn);
    // plr->SetPlayerId(pid);

    return plr;
}
