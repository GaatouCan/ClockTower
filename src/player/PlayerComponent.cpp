#include "PlayerComponent.h"
#include "ComponentModule.h"

IPlayerComponent::IPlayerComponent(UComponentModule *module)
    : module_(module){
}

IPlayerComponent::~IPlayerComponent() {
}

UComponentModule * IPlayerComponent::GetModule() const {
    return module_;
}

UPlayer * IPlayerComponent::GetOwner() const {
    if (module_ != nullptr)
        return module_->GetOwner();
    return nullptr;
}

void IPlayerComponent::OnLogin() {
}

void IPlayerComponent::OnLogout() {
}

void IPlayerComponent::SyncCache(FCacheNode *node) {
}
