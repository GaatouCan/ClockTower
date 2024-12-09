#pragma once

#include "../../player_id.h"
#include "../../character.h"
#include "../../connection.h"
#include "../../repeated_timer.h"
#include "../login/platform_info.h"

#include <mutex>
#include <shared_mutex>
#include <spdlog/spdlog.h>

class IAbstractPlayer : public UCharacter, public std::enable_shared_from_this<IAbstractPlayer> {

    class UScene *mOwnerScene;

    AConnectionPointer mConn;
    FPlayerID mPlayerID;

    ATimePoint mEnterTime;
    ATimePoint mLeaveTime;

    FPlatformInfo mPlatform;

    std::map<FUniqueID, URepeatedTimer> mTimerMap;
    std::mutex mTimerMutex;
    mutable std::shared_mutex mTimerSharedMutex;

public:
    IAbstractPlayer() = delete;

    explicit IAbstractPlayer(AConnectionPointer conn);
    ~IAbstractPlayer() override;

    bool SetConnection(AConnectionPointer conn);

    [[nodiscard]] AConnectionPointer GetConnection() const;
    [[nodiscard]] ATcpSocket &GetSocket() const;

    [[nodiscard]] AThreadID GetThreadID() const;
    bool IsSameThread() const;

    [[nodiscard]] uint32_t GetLocalID() const;
    [[nodiscard]] uint32_t GetCrossID() const;
    [[nodiscard]] const FPlayerID &GetPlayerID() const;
    [[nodiscard]] uint64_t GetFullID() const;

    [[nodiscard]] IPackage *BuildPackage() const;
    void SendPackage(IPackage *pkg) const;

    void OnEnterScene(UScene *scene);
    void OnLeaveScene(UScene *scene);

    bool TryLeaveScene();

    bool IsInScene(uint32_t id = 0) const;
    uint32_t GetCurrentSceneID() const;

    ATimePoint GetEnterSceneTime() const;
    ATimePoint GetLeaveSceneTime() const;

    void SetPlatformInfo(const FPlatformInfo &platform);
    const FPlatformInfo &GetPlatformInfo() const;

    template<typename FUNC, typename... ARGS>
    void RunInThread(FUNC &&func, ARGS &&... args) {
        co_spawn(mConn->GetSocket().get_executor(), [func = std::forward<FUNC>(func), ...args = std::forward<ARGS>(args)]() -> awaitable<void> {
            try {
                std::invoke(func, args...);
            } catch (std::exception &e) {
                spdlog::error("IAbstractPlayer::RunInThread: {}", e.what());
            }
            co_return;
        }, detached);
    }

    template<typename Functor, typename... Args>
    std::optional<FUniqueID> SetTimer(const std::chrono::duration<uint32_t> expire, const bool repeat, Functor &&func, Args &&... args) {
        FUniqueID timerID = FUniqueID::RandGenerate();
        {
            std::shared_lock lock(mTimerSharedMutex);
            while (mTimerMap.contains(timerID)) {
                timerID = FUniqueID::RandGenerate();
            }
        }

        {
            std::scoped_lock lock(mTimerMutex);
            if (auto [iter, res] = mTimerMap.insert_or_assign(timerID, GetSocket().get_executor()); res) {
                iter->second
                        .SetTimerID(timerID)
                        .SetExpireTime(expire)
                        .SetLoop(repeat)
                        .SetCallback(std::forward<Functor>(func), std::forward<Args>(args)...);

                iter->second.Start();
                return timerID;
            }
        }
        return std::nullopt;
    }

    void StopTimer(FUniqueID timerID);
    void CleanAllTimer();
};
