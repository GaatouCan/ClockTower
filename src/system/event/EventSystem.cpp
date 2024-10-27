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
} // base