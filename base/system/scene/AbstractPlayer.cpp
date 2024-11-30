//
// Created by admin on 24-11-30.
//

#include "AbstractPlayer.h"

#include <utility>

#include "Scene.h"

IAbstractPlayer::IAbstractPlayer(AConnectionPointer conn)
    : mOwnerScene(nullptr),
      mConn(std::move(conn)),
      mPlayerID(std::any_cast<uint64_t>(mConn->GetContext())) {
}

IAbstractPlayer::~IAbstractPlayer() {
}

AConnectionPointer IAbstractPlayer::GetConnection() const {
    return mConn;
}

ATcpSocket &IAbstractPlayer::GetSocket() const {
    return mConn->GetSocket();
}

IPackage *IAbstractPlayer::BuildPackage() const {
    return mConn->BuildPackage();
}

void IAbstractPlayer::Send(IPackage *pkg) const {
    mConn->Send(pkg);
}

void IAbstractPlayer::EnterScene(UScene *scene) {
    if (mOwnerScene != nullptr) {
        // TODO
    }
    mOwnerScene = scene;
    mEnterTime = std::chrono::steady_clock::now();
}

void IAbstractPlayer::LeaveScene() {
    if (mOwnerScene == nullptr)
        return;

    // TODO

    mLeaveTime = std::chrono::steady_clock::now();
    mOwnerScene = nullptr;
}


bool IAbstractPlayer::IsInScene(const uint32_t id) const {
    if (mOwnerScene == nullptr)
        return false;

    if (id == 0)
        return true;

    return mOwnerScene->GetSceneID() == id;
}
