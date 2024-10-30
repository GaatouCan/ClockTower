#include "Player.h"
#include "../base/GameWorld.h"
#include "../base/impl/Package.h"
#include "../system/event/EventSystem.h"

#include <utility>
#include <ProtoType.gen.h>
#include <login.pb.h>


UPlayer::UPlayer(AConnectionPointer conn)
    : conn_(std::move(conn)),
      id_(0),
      componentModule_(this),
      eventModule_(this) {
}

UPlayer::~UPlayer() {
}

UPlayer &UPlayer::SetPlayerId(const uint64_t id) {
    id_ = id;
    return *this;
}

uint64_t UPlayer::GetPlayerID() const {
    return id_;
}

AConnectionPointer UPlayer::GetConnection() const {
    return conn_;
}

void UPlayer::SetConnection(const AConnectionPointer &conn) {
    conn_ = conn;
}

AThreadID UPlayer::GetThreadID() const {
    return conn_->GetThreadID();
}

bool UPlayer::IsSameThread() const {
    return std::this_thread::get_id() == GetThreadID();
}

UComponentModule &UPlayer::GetComponentModule() {
    return componentModule_;
}

UEventModule &UPlayer::GetEventModule() {
    return eventModule_;
}

void UPlayer::OnLogin() {
    if (!IsSameThread()) {
        RunInThread(&UPlayer::OnLogin, this);
        return;
    }
    spdlog::info("{} - Player[{}] login succeed.", __func__, GetPlayerID());

    loginTime_ = std::chrono::steady_clock::now();
    componentModule_.OnLogin();

    Login::SC_LoginResponse response;
    response.set_result(true);
    response.set_progress(100);
    response.set_describe("Component Load Completed");

    SendPackage(SC_LoginResponse, response);

    if (const auto sys = GetSystem<UEventSystem>(); sys != nullptr) {
        const auto param = new FEventParam_PlayerLogin;
        param->pid = id_;
        sys->Dispatch(EEvent::PLAYER_LOGIN, param);
    }
}

void UPlayer::OnLogout() {
    if (!IsSameThread()) {
        RunInThread(&UPlayer::OnLogin, this);
        return;
    }

    timerMap_.clear();

    componentModule_.OnLogout();
    spdlog::info("{} - Player[{}] logout.", __func__, GetPlayerID());

    if (const auto sys = GetSystem<UEventSystem>(); sys != nullptr) {
        const auto param = new FEventParam_PlayerLogout;
        param->pid = id_;
        sys->Dispatch(EEvent::PLAYER_LOGOUT, param);
    }
}

bool UPlayer::IsLogin() const {
    return loginTime_.time_since_epoch().count() > 0;
}

void UPlayer::StopTimer(const uint64_t timerID) {
    if (const auto iter = timerMap_.find(timerID); iter != timerMap_.end()) {
        iter->second.Stop();
        timerMap_.erase(iter);
    }
}

IPackage *UPlayer::BuildPackage() const {
    return conn_->BuildPackage();
}

void UPlayer::Send(IPackage *pkg) const {
    conn_->Send(pkg);
}

void UPlayer::Send(const uint32_t id, const std::string_view data) const {
    const auto pkg = dynamic_cast<FPackage *>(conn_->BuildPackage());
    pkg->SetPackageID(id).SetData(data);
    Send(pkg);
}

void UPlayer::Send(const uint32_t id, const std::stringstream &ss) const {
    const auto pkg = dynamic_cast<FPackage *>(conn_->BuildPackage());
    pkg->SetPackageID(id).SetData(ss.str());
    Send(pkg);
}

void UPlayer::SyncCache(FCacheNode *node) {
    componentModule_.SyncCache(node);
}

std::shared_ptr<UPlayer> CreatePlayer(const AConnectionPointer &conn, const uint64_t pid) {
    auto plr = std::make_shared<UPlayer>(conn);
    plr->SetPlayerId(pid);

    return plr;
}
