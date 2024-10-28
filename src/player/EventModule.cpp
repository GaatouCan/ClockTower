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

void EventModule::RegisterListener(const Event event, void *ptr, const EventListener &listener) {

}

void EventModule::RemoveListener(Event event, void *ptr) {

}
