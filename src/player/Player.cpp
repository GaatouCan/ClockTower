#include "Player.h"
#include "../base/impl/Package.h"

#include <utility>


Player::Player(base::ConnectionPointer conn)
    : conn_(std::move(conn)),
      id_(0),
      module_(this) {
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

ComponentModule & Player::GetComponentModule() {
    return module_;
}

void Player::OnLogin() {
    if (!IsSameThread()) {
        RunInThread(&Player::OnLogin, this);
        return;
    }
    loginTime_ = std::chrono::steady_clock::now();
    module_.OnLogin();
}

void Player::OnLogout() {
    if (!IsSameThread()) {
        RunInThread(&Player::OnLogin, this);
        return;
    }

    module_.OnLogout();
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

base::IPackage * Player::BuildPackage() const {
    return conn_->BuildPackage();
}

void Player::Send(base::IPackage *pkg) const {
    conn_->Send(pkg);
}

void Player::Send(const uint32_t id, const std::string_view data) const {
    const auto pkg = dynamic_cast<base::Package *>(conn_->BuildPackage());
    pkg->SetPackageID(id).SetData(data);
    Send(pkg);
}

void Player::Send(const uint32_t id, const std::stringstream &ss) const {
    const auto pkg = dynamic_cast<base::Package *>(conn_->BuildPackage());
    pkg->SetPackageID(id).SetData(ss.str());
    Send(pkg);
}

void Player::SyncCache(CacheNode *node) {
    module_.SyncCache(node);
}

std::shared_ptr<Player> CreatePlayer(const base::ConnectionPointer &conn, const uint64_t pid) {
    auto plr = std::make_shared<Player>(conn);
    plr->SetPlayerId(pid);

    return plr;
}
