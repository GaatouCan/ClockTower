#pragma once

#include "../base/Character.h"
#include "../base/Connection.h"
#include "../base/RepeatedTimer.h"
#include "../base/utils.h"
#include "../base/system/login/PlatformInfo.h"
#include "../base/system/event/EventParam.h"

#include "ComponentModule.h"
#include "EventModule.h"

#include <spdlog/spdlog.h>

struct FEP_PlayerLogin final : IEventParam {
    uint64_t pid;
};

struct FEP_PlayerLogout final : IEventParam {
    uint64_t pid;
};

class UPlayer final : public UCharacter, public std::enable_shared_from_this<UPlayer> {

    AConnectionPointer mConn;
    uint64_t mId;

    UComponentModule mComponentModule;
    UEventModule mEventModule;

    ATimePoint mLoginTime;
    FPlatformInfo mPlatform;

    std::map<uint64_t, URepeatedTimer> mTimerMap;

public:
    UPlayer() = delete;

    explicit UPlayer(AConnectionPointer conn);
    ~UPlayer() override;

    UPlayer &SetPlayerId(uint64_t id);
    uint64_t GetPlayerID() const;

    AConnectionPointer GetConnection() const;
    void SetConnection(const AConnectionPointer &conn);

    [[nodiscard]] AThreadID GetThreadID() const;

    bool IsSameThread() const;

    template<typename FUNC, typename... ARGS>
    void RunInThread(FUNC &&func, ARGS &&... args) {
        co_spawn(mConn->GetSocket().get_executor(), [func = std::forward<FUNC>(func), ...args = std::forward<ARGS>(args)]() -> awaitable<void> {
            try {
                std::invoke(func, args...);
            } catch (std::exception &e) {
                spdlog::error("Player::RunInThread: {}", e.what());
            }
            co_return;
        }, detached);
    }

    UComponentModule &GetComponentModule();
    UEventModule &GetEventModule();

    void OnLogin();
    void OnLogout();

    bool IsLogin() const;

    template<typename FUNC, typename... ARGS>
    uint64_t SetTimer(const std::chrono::duration<uint32_t> expire, const bool repeat, FUNC &&func, ARGS &&... args) {
        const uint64_t timerID = UnixTime();
        if (auto [iter, res] = mTimerMap.insert_or_assign(timerID, mConn->GetSocket().get_executor()); res) {
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

    IPackage *BuildPackage() const;
    void Send(IPackage *pkg) const;

    void Send(uint32_t id, std::string_view data) const;
    void Send(uint32_t id, const std::stringstream &ss) const;

    void SyncCache(struct FCacheNode *node);
};

std::shared_ptr<UPlayer> CreatePlayer(const AConnectionPointer &, uint64_t);

#define SendPackage(proto, data) \
    Send(static_cast<uint32_t>(protocol::EProtoType::proto), data.SerializeAsString())
