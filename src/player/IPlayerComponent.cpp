#include "IPlayerComponent.h"
#include "ComponentModule.h"

IPlayerComponent::IPlayerComponent(ComponentModule *module)
    : module_(module){
}

IPlayerComponent::~IPlayerComponent() {
}

ComponentModule * IPlayerComponent::GetModule() const {
    return module_;
}

Player * IPlayerComponent::GetOwner() const {
    if (module_ != nullptr)
        return module_->GetOwner();
    return nullptr;
}
