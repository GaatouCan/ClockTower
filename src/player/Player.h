#pragma once

#include "../base/ICharacter.h"
#include "../base/Connection.h"

class Player : public ICharacter, public std::enable_shared_from_this<Player> {

    base::ConnectionPointer conn_;
    uint64_t id_;

public:
    Player() = delete;

    explicit Player(base::ConnectionPointer conn);
    ~Player() override;

    Player& setPlayerId(uint64_t id);
    uint64_t pid() const;
};
