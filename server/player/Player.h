#pragma once

#include "ComponentModule.h"
#include "EventModule.h"

#include <Character.h>
#include <Connection.h>
#include <RepeatedTimer.h>
#include <utils.h>
#include <system/login/PlatformInfo.h>
#include <system/event/EventParam.h>

#include <spdlog/spdlog.h>

struct FEP_PlayerLogin final : IEventParam {
    uint64_t pid;
};

struct FEP_PlayerLogout final : IEventParam {
    uint64_t pid;
};

class UPlayer final : public UCharacter {

    AConnectionPointer mConn;
    uint64_t mId;

    ATimePoint mLoginTime;
    ATimePoint mLogoutTime;

    UComponentModule mComponentModule;
    UEventModule mEventModule;

    std::map<uint64_t, URepeatedTimer> mTimerMap;

    FPlatformInfo mPlatform;

public:
    UPlayer() = delete;

    explicit UPlayer(AConnectionPointer conn);
    ~UPlayer() override;

    void SetConnection(const AConnectionPointer &conn);
    AConnectionPointer GetConnection() const;

    ATcpSocket &GetSocket() const;

    uint64_t GetPlayerID() const;

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

    [[nodiscard]] AThreadID GetThreadID() const;

    bool IsSameThread() const;

    UComponentModule &GetComponentModule();
    UEventModule &GetEventModule();

    awaitable<void> OnLogin();
    void OnLogout();

    bool IsOnline() const;

    template<typename FUNC, typename... ARGS>
    uint64_t SetTimer(const std::chrono::duration<uint32_t> expire, const bool repeat, FUNC &&func, ARGS &&... args) {
        const uint64_t timerID = UnixTime();
        if (auto [iter, res] = mTimerMap.insert_or_assign(timerID, GetSocket().get_executor()); res) {
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

#define SEND_PACKAGE(sender, proto, data) \
    (sender)->Send(static_cast<uint32_t>(protocol::EProtoType::proto), data.SerializeAsString());

#define BUILD_PACKAGE(pkg, proto, data) \
    pkg->SetPackageID(static_cast<uint32_t>(protocol::EProtoType::proto)).SetData(data.SerializeAsString());
