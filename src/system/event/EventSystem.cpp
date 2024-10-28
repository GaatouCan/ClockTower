#include "EventSystem.h"

#include "../../base/GameWorld.h"

namespace base {
    EventSystem::~EventSystem() {
        listenerMap_.clear();

        while (!queue_->empty()) {
            auto &[event, param] = queue_->front();
            queue_->pop();
            delete param;
        }
    }

    void EventSystem::Init() {
    }

    awaitable<void> EventSystem::HandleEvent() {
        while (!IsQueueEmpty()) {
            EventNode node;

            {
                std::scoped_lock lock(eventMutex_);
                node = queue_->front();
                queue_->pop();
            }

            if (node.event == Event::UNAVAILABLE) {
                delete node.param;
                continue;
            }

            curListener_.clear();

            // 拷贝一份map 减少锁的范围 可以在调用事件处理函数时修改注册map
            {
                std::scoped_lock lock(listenerMutex_);
                if (const auto iter = listenerMap_.find(node.event); iter != listenerMap_.end()) {
                    curListener_ = iter->second;
                }
            }

            for (const auto& listener : std::views::values(curListener_)) {
                std::invoke(listener, node.param);
            }

            delete node.param;
        }

        co_return;
    }

    bool EventSystem::IsQueueEmpty() const {
        std::shared_lock lock(sharedMutex_);
        return queue_->empty();
    }

    void EventSystem::Dispatch(const Event event, IEventParam *parma) {
        const bool empty = IsQueueEmpty();

        {
            std::scoped_lock lock(eventMutex_);
            queue_->emplace(event, parma);
        }

        if (empty)
            co_spawn(GetWorld().GetIOContext(), HandleEvent(), detached);
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