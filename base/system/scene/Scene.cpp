#include "Scene.h"
#include "AbstractPlayer.h"

std::function<APlayerPointer(const AConnectionPointer &)> UScene::sPlayerCreator = nullptr;

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

APlayerPointer UScene::CreatePlayer(const AConnectionPointer &conn) {
    if (sPlayerCreator == nullptr)
        return nullptr;

    auto plr = std::invoke(sPlayerCreator, conn);
    PlayerEnterScene(plr);

    return plr;
}

void UScene::PlayerEnterScene(const APlayerPointer &player) {
    if (std::this_thread::get_id() != GetWorld().GetThreadID()) {
        RunInThread(&UScene::PlayerEnterScene, this, player);
        return;
    }

    if (player == nullptr)
        return;

    if (mPlayerMap.contains(player->GetPlayerID().ToUInt64()))
        return;

    mPlayerMap[player->GetPlayerID().ToUInt64()] = player;
    player->OnEnterScene(this);
}

void UScene::PlayerLeaveScene(const APlayerPointer &player) {
    if (std::this_thread::get_id() != GetWorld().GetThreadID()) {
        RunInThread(&UScene::PlayerLeaveScene, this, player);
        return;
    }

    if (player == nullptr)
        return;

    if (!mPlayerMap.contains(player->GetPlayerID().ToUInt64()))
        return;

    mPlayerMap.erase(player->GetPlayerID().ToUInt64());
    player->OnLeaveScene(this);
}

void UScene::DefinePlayerCreator(const std::function<APlayerPointer(const AConnectionPointer &)> &creator) {
    sPlayerCreator = creator;
}
