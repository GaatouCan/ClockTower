//
// Created by admin on 24-11-30.
//

#include "abstract_player.h"

#include <utility>

#include "scene.h"

IAbstractPlayer::IAbstractPlayer(AConnectionPointer conn)
    : mOwnerScene(nullptr),
      mConn(std::move(conn)),
      mPlayerID(std::any_cast<FPlayerID>(mConn->GetContext())) {
}

IAbstractPlayer::~IAbstractPlayer() {

}

bool IAbstractPlayer::SetConnection(AConnectionPointer conn) {
    if (std::any_cast<FPlayerID>(mConn->GetContext()) != mPlayerID) {
        return false;
    }
    mConn = std::move(conn);
    return true;
}

AConnectionPointer IAbstractPlayer::GetConnection() const {
    return mConn;
}

ATcpSocket &IAbstractPlayer::GetSocket() const {
    return mConn->GetSocket();
}

AThreadID IAbstractPlayer::GetThreadID() const {
    return mConn->GetThreadID();
}

bool IAbstractPlayer::IsSameThread() const {
    return std::this_thread::get_id() == GetThreadID();
}

uint32_t IAbstractPlayer::GetLocalID() const {
    return mPlayerID.GetLocalID();
}

uint32_t IAbstractPlayer::GetCrossID() const {
    return mPlayerID.GetCrossID();
}

const FPlayerID &IAbstractPlayer::GetPlayerID() const {
    return mPlayerID;
}

uint64_t IAbstractPlayer::GetFullID() const {
    return mPlayerID.ToUInt64();
}

IPackage *IAbstractPlayer::BuildPackage() const {
    return mConn->BuildPackage();
}

void IAbstractPlayer::SendPackage(IPackage *pkg) const {
    mConn->Send(pkg);
}

void IAbstractPlayer::OnEnterScene(UScene *scene) {
    if (mOwnerScene != nullptr) {
        // TODO
    }
    mOwnerScene = scene;
    mEnterTime = NowTimePoint();
}

void IAbstractPlayer::OnLeaveScene(UScene *scene) {
    if (mOwnerScene == nullptr)
        return;

    // TODO

    mLeaveTime = NowTimePoint();
    mOwnerScene = nullptr;
}

bool IAbstractPlayer::TryLeaveScene() {
    if (mOwnerScene == nullptr) {
        return false;
    }
    mOwnerScene->PlayerLeaveScene(shared_from_this());
    return true;
}

bool IAbstractPlayer::IsInScene(const uint32_t id) const {
    if (mOwnerScene == nullptr)
        return false;

    if (id == 0)
        return true;

    return mOwnerScene->GetSceneID() == id;
}

uint32_t IAbstractPlayer::GetCurrentSceneID() const {
    if (mOwnerScene == nullptr)
        return 0;
    return mOwnerScene->GetSceneID();
}

ATimePoint IAbstractPlayer::GetEnterSceneTime() const {
    return mEnterTime;
}

ATimePoint IAbstractPlayer::GetLeaveSceneTime() const {
    return mLeaveTime;
}

void IAbstractPlayer::SetPlatformInfo(const FPlatformInfo &platform) {
    mPlatform = platform;
}

const FPlatformInfo &IAbstractPlayer::GetPlatformInfo() const {
    return mPlatform;
}

void IAbstractPlayer::StopTimer(const FUniqueID timerID) {
    std::scoped_lock lock(mTimerMutex);
    if (const auto iter = mTimerMap.find(timerID); iter != mTimerMap.end()) {
        iter->second.Stop();
        mTimerMap.erase(iter);
    }
}

void IAbstractPlayer::CleanAllTimer() {
    std::scoped_lock lock(mTimerMutex);
    mTimerMap.clear();
}
