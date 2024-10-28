#include "EventModule.h"
#include "Player.h"

EventModule::EventModule(Player *plr)
    : owner_(plr) {
}

EventModule::~EventModule() {
}

Player * EventModule::GetOwner() const {
    return owner_;
}
