#pragma once

#include "../base/ICharacter.h"
#include "../base/Connection.h"
#include "ComponentModule.h"
#include "../system/login/PlatformInfo.h"

#include "../base/RepeatedTimer.h"
#include "../common/utils.h"

#include <spdlog/spdlog.h>

class Player final : public ICharacter, public std::enable_shared_from_this<Player> {

    base::ConnectionPointer conn_;
    uint64_t id_;

    ComponentModule componentModule_;

    TimePoint loginTime_;
    base::PlatformInfo platform_;

    std::map<uint64_t, base::RepeatedTimer> timerMap_;

public:
    Player() = delete;

    explicit Player(base::ConnectionPointer conn);
    ~Player() override;

    Player &SetPlayerId(uint64_t id);
    uint64_t GetPlayerID() const;

    base::ConnectionPointer GetConnection() const;
    void SetConnection(const base::ConnectionPointer &conn);

    [[nodiscard]] ThreadID GetThreadID() const;

    bool IsSameThread() const;

    template<typename FUNC, typename... ARGS>
    void RunInThread(FUNC &&func, ARGS &&... args) {
        co_spawn(conn_->GetSocket().get_executor(), [func = std::forward<FUNC>(func), ...args = std::forward<ARGS>(args)]() -> awaitable<void> {
            try {
                std::invoke(func, args...);
            } catch (std::exception &e) {
                spdlog::error("Player::RunInThread: {}", e.what());
            }
            co_return;
        }, detached);
    }

    ComponentModule &GetComponentModule();

    void OnLogin();
    void OnLogout();

    bool IsLogin() const;

    template<typename FUNC, typename... ARGS>
    uint64_t SetTimer(const std::chrono::duration<uint32_t> expire, const bool repeat, FUNC &&func, ARGS &&... args) {
        const uint64_t timerID = CurrentTimeCount();
        if (auto [iter, res] = timerMap_.insert_or_assign(timerID, conn_->GetSocket().get_executor()); res) {
            iter->second
                    .SetTimerID(timerID)
                    .SetExpireTime(expire)
                    .SetLoop(repeat)
                    .SetCallback(std::forward<FUNC>(func), std::forward<ARGS>(args)...);

            iter->second.Start();
            return timerID;
        }
        return 0;
    }

    void StopTimer(uint64_t timerID);

    base::IPackage *BuildPackage() const;
    void Send(base::IPackage *pkg) const;

    void Send(uint32_t id, std::string_view data) const;
    void Send(uint32_t id, const std::stringstream &ss) const;

    void SyncCache(struct CacheNode *node);
};

std::shared_ptr<Player> CreatePlayer(const base::ConnectionPointer &, uint64_t);

#define SendPackage(proto, data) \
    Send(static_cast<uint32_t>(protocol::ProtoType::proto), data.SerializeAsString())
