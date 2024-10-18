#include "Player.h"

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

ThreadID Player::GetThreadID() const {
    return conn_->GetThreadID();
}

bool Player::IsSameThread() const {
    return std::this_thread::get_id() == GetThreadID();
}

void Player::OnLogin() {
    module_.OnLogin();
}

void Player::OnLogout() {
    if (!IsSameThread()) {
        RunInThread(&Player::OnLogin, this);
        return;
    }

    module_.OnLogout();
}

void Player::StopTimer(const uint64_t timerID) {
    if (const auto iter = timerMap_.find(timerID); iter != timerMap_.end()) {
        iter->second.Stop();
        timerMap_.erase(iter);
    }
}

std::shared_ptr<Player> CreatePlayer(const base::ConnectionPointer &conn, const uint64_t pid) {
    auto plr = std::make_shared<Player>(conn);
    plr->SetPlayerId(pid);

    return plr;
}
