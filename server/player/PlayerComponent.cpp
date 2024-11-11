#include "PlayerComponent.h"
#include "ComponentModule.h"

IPlayerComponent::IPlayerComponent(UComponentModule *module)
    : mModule(module){
}

IPlayerComponent::~IPlayerComponent() {
}

UComponentModule * IPlayerComponent::GetModule() const {
    return mModule;
}

UPlayer * IPlayerComponent::GetOwner() const {
    if (mModule != nullptr)
        return mModule->GetOwner();
    return nullptr;
}

void IPlayerComponent::OnLogin() {
}

void IPlayerComponent::OnLogout() {
}

void IPlayerComponent::SyncCache(FCacheNode *node) {
}
