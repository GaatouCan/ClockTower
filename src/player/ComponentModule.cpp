#include "ComponentModule.h"
#include "Player.h"

ComponentModule::ComponentModule(Player *plr)
    : owner_(plr){
}

ComponentModule::~ComponentModule() {
    for (const auto& [comp, serialize] : std::views::values(componentMap_)) {
        delete comp;
    }
}

Player * ComponentModule::GetOwner() const {
    return owner_;
}

void ComponentModule::OnLogin() {
    for (const auto& [comp, serialize] : std::views::values(componentMap_)) {
        comp->OnLogin();
    }
}

void ComponentModule::OnLogout() {
    for (const auto& [comp, serialize] : std::views::values(componentMap_)) {
        comp->OnLogout();
    }
}
