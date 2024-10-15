#include "Player.h"

#include <utility>

Player::Player(base::ConnectionPointer conn)
    : conn_(std::move(conn)),
      id_(0) {
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
