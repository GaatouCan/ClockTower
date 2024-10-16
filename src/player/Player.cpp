#include "Player.h"

#include <utility>

Player::Player(base::ConnectionPointer conn)
    : conn_(std::move(conn)),
      id_(0),
      threadId_(std::this_thread::get_id()) {
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

std::thread::id Player::GetThreadID() const {
    return threadId_;
}

bool Player::IsSameThread() const {
    return std::this_thread::get_id() == threadId_;
}

std::shared_ptr<Player> CreatePlayer(const base::ConnectionPointer &conn, const uint64_t pid) {
    auto plr = std::make_shared<Player>(conn);
    plr->SetPlayerId(pid);

    return plr;
}