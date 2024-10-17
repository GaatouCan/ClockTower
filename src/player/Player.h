#pragma once

#include "../base/ICharacter.h"
#include "../base/Connection.h"
#include "ComponentModule.h"

class Player final : public ICharacter, public std::enable_shared_from_this<Player> {

    base::ConnectionPointer conn_;
    uint64_t id_;

    ComponentModule module_;

public:
    Player() = delete;

    explicit Player(base::ConnectionPointer conn);
    ~Player() override;

    Player& SetPlayerId(uint64_t id);
    uint64_t GetPlayerID() const;

    base::ConnectionPointer GetConnection() const;

    [[nodiscard]] ThreadID GetThreadID() const;
    bool IsSameThread() const;

    template<typename FUNC, typename ... ARGS>
    void RunInThread(FUNC &&func, ARGS &&... args) {
        co_spawn(conn_->GetSocket().get_executor(), [func = std::forward<FUNC>(func), ...args = std::forward<ARGS>(args)]() -> awaitable<void> {
            std::invoke(func, args...);
            co_return;
        }, detached);
    }

    void OnLogin();
    void OnLogout();
};

std::shared_ptr<Player> CreatePlayer(const base::ConnectionPointer&, uint64_t);
