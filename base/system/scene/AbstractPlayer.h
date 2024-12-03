#pragma once

#include "../../PlayerID.h"
#include "../../Character.h"
#include "../../Connection.h"
#include "../login/PlatformInfo.h"

#include <spdlog/spdlog.h>

class IAbstractPlayer : public UCharacter, public std::enable_shared_from_this<IAbstractPlayer> {

    class UScene *mOwnerScene;

    AConnectionPointer mConn;
    FPlayerID mPlayerID;

    ATimePoint mEnterTime;
    ATimePoint mLeaveTime;

    FPlatformInfo mPlatform;

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
    void Send(IPackage *pkg) const;

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
};
