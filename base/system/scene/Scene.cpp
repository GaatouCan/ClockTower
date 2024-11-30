#include "Scene.h"
#include "AbstractPlayer.h"

std::function<std::shared_ptr<IAbstractPlayer>(const std::shared_ptr<UConnection> &)> UScene::sPlayerCreator = nullptr;

UScene::UScene() {
}

UScene::~UScene() {
}

void UScene::SetSceneID(uint32_t id) {
    mSceneID = id;
}

uint32_t UScene::GetSceneID() const {
    return mSceneID;
}

std::shared_ptr<IAbstractPlayer> UScene::CreatePlayer(const std::shared_ptr<UConnection> &conn) {
    if (sPlayerCreator == nullptr)
        return nullptr;

    auto plr = std::invoke(sPlayerCreator, conn);
    PlayerEnterScene(plr);

    return plr;
}

void UScene::PlayerEnterScene(const std::shared_ptr<IAbstractPlayer> &player) {
    if (std::this_thread::get_id() != GetWorld().GetThreadID()) {
        RunInThread(&UScene::PlayerEnterScene, this, player);
        return;
    }

    if (player == nullptr)
        return;

    if (mPlayerMap.contains(player->GetPlayerID()))
        return;

    mPlayerMap[player->GetPlayerID()] = player;
    player->OnEnterScene(this);
}

void UScene::PlayerLeaveScene(const std::shared_ptr<IAbstractPlayer> &player) {
    if (std::this_thread::get_id() != GetWorld().GetThreadID()) {
        RunInThread(&UScene::PlayerLeaveScene, this, player);
        return;
    }

    if (player == nullptr)
        return;

    if (!mPlayerMap.contains(player->GetPlayerID()))
        return;

    mPlayerMap.erase(player->GetPlayerID());
    player->OnLeaveScene(this);
}

void UScene::DefinePlayerCreator(const std::function<std::shared_ptr<IAbstractPlayer>(const std::shared_ptr<UConnection> &)> &creator) {
    sPlayerCreator = creator;
}
