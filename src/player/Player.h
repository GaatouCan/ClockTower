#pragma once

#include "../base/ICharacter.h"
#include "../base/Connection.h"

class Player : public ICharacter, public std::enable_shared_from_this<Player> {

    base::ConnectionPointer conn_;
    uint64_t id_;

    std::thread::id threadId_;

public:
    Player() = delete;

    explicit Player(base::ConnectionPointer conn);
    ~Player() override;

    Player& setPlayerId(uint64_t id);
    uint64_t pid() const;

    [[nodiscard]] std::thread::id threadId() const;
    bool isInSameThread() const;

    template<typename FUNC, typename ... ARGS>
    void runInThread(FUNC &&func, ARGS &&... args) {
        co_spawn(conn_->GetSocket().get_executor(), [func = std::move(func), ...args = std::forward<ARGS>(args)]() -> awaitable<void> {
            std::invoke(func, args...);
            co_return;
        }, detached);
    }
};
