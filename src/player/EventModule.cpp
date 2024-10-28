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

bool EventModule::IsQueueEmpty() const {
    std::shared_lock lock(sharedMutex_);
    return queue_->empty();
}

void EventModule::RegisterListener(const Event event, void *ptr, const EventListener &listener) {
    if (event == Event::UNAVAILABLE || ptr == nullptr)
        return;

    std::scoped_lock lock(listenerMutex_);
    if (!listenerMap_.contains(event))
        listenerMap_[event] = std::map<void *, EventListener>();

    listenerMap_[event][ptr] = listener;
}

void EventModule::RemoveListener(const Event event, void *ptr) {
    if (ptr == nullptr)
        return;

    std::scoped_lock lock(listenerMutex_);
    if (event == Event::UNAVAILABLE) {
        for (auto &val : std::views::values(listenerMap_)) {
            val.erase(ptr);
        }
    } else {
        if (const auto iter = listenerMap_.find(event); iter != listenerMap_.end()) {
            iter->second.erase(ptr);
        }
    }
}
