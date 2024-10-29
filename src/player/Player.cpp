#include "Player.h"
#include "../base/GameWorld.h"
#include "../base/impl/Package.h"
#include "../system/event/EventSystem.h"
#include "../../protobuf/ProtoType.generated.h"

#include <utility>
#include <login.pb.h>


Player::Player(base::ConnectionPointer conn)
    : conn_(std::move(conn)),
      id_(0),
      componentModule_(this),
      eventModule_(this) {
}

Player::~Player() {
}

Player &Player::SetPlayerId(const uint64_t id) {
    id_ = id;
    return *this;
}

uint64_t Player::GetPlayerID() const {
    return id_;
}

base::ConnectionPointer Player::GetConnection() const {
    return conn_;
}

void Player::SetConnection(const base::ConnectionPointer &conn) {
    conn_ = conn;
}

ThreadID Player::GetThreadID() const {
    return conn_->GetThreadID();
}

bool Player::IsSameThread() const {
    return std::this_thread::get_id() == GetThreadID();
}

ComponentModule &Player::GetComponentModule() {
    return componentModule_;
}

EventModule &Player::GetEventModule() {
    return eventModule_;
}

void Player::OnLogin() {
    if (!IsSameThread()) {
        RunInThread(&Player::OnLogin, this);
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

    if (const auto sys = GetSystem<base::EventSystem>(); sys != nullptr) {
        const auto param = new EP_PlayerLogin;
        param->pid = id_;
        sys->Dispatch(Event::PLAYER_LOGIN, param);
    }
}

void Player::OnLogout() {
    if (!IsSameThread()) {
        RunInThread(&Player::OnLogin, this);
        return;
    }

    timerMap_.clear();

    componentModule_.OnLogout();
    spdlog::info("{} - Player[{}] logout.", __func__, GetPlayerID());

    if (const auto sys = GetSystem<base::EventSystem>(); sys != nullptr) {
        const auto param = new EP_PlayerLogout;
        param->pid = id_;
        sys->Dispatch(Event::PLAYER_LOGOUT, param);
    }
}

bool Player::IsLogin() const {
    return loginTime_.time_since_epoch().count() > 0;
}

void Player::StopTimer(const uint64_t timerID) {
    if (const auto iter = timerMap_.find(timerID); iter != timerMap_.end()) {
        iter->second.Stop();
        timerMap_.erase(iter);
    }
}

base::IPackage *Player::BuildPackage() const {
    return conn_->BuildPackage();
}

void Player::Send(base::IPackage *pkg) const {
    conn_->Send(pkg);
}

void Player::Send(const uint32_t id, const std::string_view data) const {
    const auto pkg = dynamic_cast<base::FPackage *>(conn_->BuildPackage());
    pkg->SetPackageID(id).SetData(data);
    Send(pkg);
}

void Player::Send(const uint32_t id, const std::stringstream &ss) const {
    const auto pkg = dynamic_cast<base::FPackage *>(conn_->BuildPackage());
    pkg->SetPackageID(id).SetData(ss.str());
    Send(pkg);
}

void Player::SyncCache(CacheNode *node) {
    componentModule_.SyncCache(node);
}

std::shared_ptr<Player> CreatePlayer(const base::ConnectionPointer &conn, const uint64_t pid) {
    auto plr = std::make_shared<Player>(conn);
    plr->SetPlayerId(pid);

    return plr;
}
