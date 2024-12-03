#include "Scene.h"
#include "AbstractPlayer.h"
#include "SceneSystem.h"

UScene::UScene(const uint32_t id)
    : mSceneID(id) {
}

UScene::~UScene() {
}

void UScene::SetSceneID(const uint32_t id) {
    mSceneID = id;
}

uint32_t UScene::GetSceneID() const {
    return mSceneID;
}

void UScene::PlayerEnterScene(const std::shared_ptr<IAbstractPlayer> &player) {
    // if (std::this_thread::get_id() != GetWorld().GetThreadID()) {
    //     RunInThread(&UScene::PlayerEnterScene, this, player);
    //     return;
    // }

    if (player == nullptr)
        return;

    {
        std::shared_lock lock(mSharedMutex);
        if (mPlayerMap.contains(player->GetPlayerID()))
            return;
    }

    // Change Scene From Other Scene
    if (const auto otherSceneID = player->GetCurrentSceneID(); otherSceneID != 0 && otherSceneID != mSceneID) {
        if (const auto sys = GetSystem<USceneSystem>(); sys != nullptr) {
            if (const auto scene = sys->GetSceneByID(otherSceneID); scene != nullptr && scene != this) {
                scene->PlayerLeaveScene(player, true);
            }
        }
    }

    {
        std::scoped_lock lock(mMutex);
        mPlayerMap[player->GetPlayerID()] = player;
    }
    player->OnEnterScene(this);
}

void UScene::PlayerLeaveScene(const std::shared_ptr<IAbstractPlayer> &player, const bool bChange) {
    // if (std::this_thread::get_id() != GetWorld().GetThreadID()) {
    //     RunInThread(&UScene::PlayerLeaveScene, this, player, bChange);
    //     return;
    // }

    if (player == nullptr)
        return;

    {
        std::scoped_lock lock(mSharedMutex);

        if (!mPlayerMap.contains(player->GetPlayerID()))
            return;

        mPlayerMap.erase(player->GetPlayerID());
    }

    if (!bChange)
        player->OnLeaveScene(this);
}

std::shared_ptr<IAbstractPlayer> UScene::GetPlayer(const FPlayerID &pid) const {
    if (!pid.IsValid())
        return nullptr;

    std::shared_lock lock(mSharedMutex);
    if (const auto it = mPlayerMap.find(pid); it != mPlayerMap.end()) {
        return it->second;
    }
    return nullptr;
}
