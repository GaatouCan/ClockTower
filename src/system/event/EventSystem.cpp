#include "EventSystem.h"

#include "../../base/GameWorld.h"

namespace base {
    void EventSystem::Init() {
    }

    awaitable<void> EventSystem::HandleEvent() {
        {
            std::scoped_lock lock(eventMutex_);
            curQueue_.swap(waitQueue_);
        }

        handling_ = true;

        while (!curQueue_->empty()) {
            auto [event, param] = curQueue_->front();
            curQueue_->pop();

            // TODO: handle event
        }

        co_return;
    }

    void EventSystem::Dispatch(const Event event, IEventParam *parma) {
        {
            std::scoped_lock lock(eventMutex_);
            waitQueue_->emplace(EventNode(event, parma));
        }

        if (!handling_) {
            co_spawn(GetWorld().GetIOContext(),  HandleEvent(), detached);
        }
    }

    void EventSystem::RegisterListener(const Event event, void *ptr, const EventListener &listener) {
        if (event == Event::UNAVAILABLE || ptr == nullptr)
            return;

        std::scoped_lock lock(listenerMutex_);
        if (!listenerMap_.contains(event))
            listenerMap_[event] = std::map<void *, EventListener>();

        listenerMap_[event][ptr] = listener;
    }

    void EventSystem::RemoveListener(const Event event, void *ptr) {
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
} // base