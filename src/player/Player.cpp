#include "Player.h"

#include <utility>

Player::Player(base::ConnectionPointer conn)
    : conn_(std::move(conn)),
      id_(0),
      threadId_(std::this_thread::get_id()) {
}

Player::~Player() {
}

Player &Player::setPlayerId(const uint64_t id) {
    id_ = id;
    return *this;
}

uint64_t Player::pid() const {
    return id_;
}

std::thread::id Player::threadId() const {
    return threadId_;
}

bool Player::isInSameThread() const {
    return std::this_thread::get_id() == threadId_;
}
