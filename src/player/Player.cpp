#include "Player.h"

#include <utility>

Player::Player(base::ConnectionPointer conn)
    : conn_(std::move(conn)),
      id_(0),
      tid_(conn_->GetThreadID()) {
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
    return tid_;
}

bool Player::IsSameThread() const {
    return std::this_thread::get_id() == tid_;
}

void Player::OnLogin() {
}

void Player::OnLogout() {
    if (!IsSameThread()) {
        RunInThread(&Player::OnLogin, this);
        return;
    }
}

std::shared_ptr<Player> CreatePlayer(const base::ConnectionPointer &conn, const uint64_t pid) {
    auto plr = std::make_shared<Player>(conn);
    plr->SetPlayerId(pid);

    return plr;
}
