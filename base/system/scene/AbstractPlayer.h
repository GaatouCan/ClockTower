#pragma once

#include "../../Character.h"
#include "../../Connection.h"
#include "../login/PlatformInfo.h"

class IAbstractPlayer : public UCharacter, public std::enable_shared_from_this<IAbstractPlayer> {

    class UScene *mOwnerScene;

    AConnectionPointer mConn;
    uint64_t mPlayerID;

    ATimePoint mEnterTime;
    ATimePoint mLeaveTime;

    FPlatformInfo mPlatform;

public:
    IAbstractPlayer() = delete;

    explicit IAbstractPlayer(AConnectionPointer conn);
    ~IAbstractPlayer() override;

    [[nodiscard]] AConnectionPointer GetConnection() const;
    [[nodiscard]] ATcpSocket &GetSocket() const;
    [[nodiscard]] uint64_t GetPlayerID() const;

    [[nodiscard]] IPackage *BuildPackage() const;
    void Send(IPackage *pkg) const;

    void OnEnterScene(UScene *scene);
    void OnLeaveScene(UScene *scene);

    bool IsInScene(uint32_t id = 0) const;
    uint32_t GetCurrentSceneID() const;
};

using APlayerPointer = std::shared_ptr<IAbstractPlayer>;
