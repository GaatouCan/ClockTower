#include "SceneSystem.h"
#include "Scene.h"

constexpr int kSceneCount = 10;

USceneSystem::~USceneSystem() {
    for (const auto scene : mSceneVector) {
        delete scene;
    }
}

void USceneSystem::Init() {
    for (uint32_t index = 0; index < kSceneCount; index++) {
        auto scene = new UScene(index + 1);
        // scene->SetSceneID(index + 1);
        mSceneVector.emplace_back(scene);
    }
}

UScene * USceneSystem::GetMainScene() const {
    return GetSceneByID(1);
}

UScene * USceneSystem::GetSceneByID(const uint32_t id) const {
    if (id == 0 || id > mSceneVector.size()) {
        return nullptr;
    }
    return mSceneVector[id - 1];
}
