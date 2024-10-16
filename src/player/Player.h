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

    Player& SetPlayerId(uint64_t id);
    uint64_t GetPlayerID() const;

    [[nodiscard]] std::thread::id GetThreadID() const;
    bool IsSameThread() const;

    template<typename FUNC, typename ... ARGS>
    void RunInThread(FUNC &&func, ARGS &&... args) {
        co_spawn(conn_->GetSocket().get_executor(), [func = std::forward<FUNC>(func), ...args = std::forward<ARGS>(args)]() -> awaitable<void> {
            std::invoke(func, args...);
            co_return;
        }, detached);
    }
};
