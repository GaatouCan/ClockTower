#include "ComponentModule.h"
#include "Player.h"

ComponentModule::ComponentModule(Player *plr)
    : owner_(plr){
}

ComponentModule::~ComponentModule() {
}

Player * ComponentModule::GetOwner() const {
    return owner_;
}
