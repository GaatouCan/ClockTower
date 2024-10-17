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
