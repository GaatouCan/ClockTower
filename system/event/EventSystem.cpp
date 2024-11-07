#include "EventSystem.h"

#include "../../base/GameWorld.h"

UEventSystem::UEventSystem() {
}


UEventSystem::~UEventSystem() {
    listenerMap_.clear();

    while (!queue_.empty()) {
        auto &[event, param] = queue_.front();
        queue_.pop();
        delete param;
    }
}

void UEventSystem::Init() {
}

awaitable<void> UEventSystem::HandleEvent() {
    while (!IsQueueEmpty()) {
        EventNode node; {
            std::scoped_lock lock(eventMutex_);
            node = queue_.front();
            queue_.pop();
        }

        if (node.event == EEvent::UNAVAILABLE) {
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

        for (const auto &listener: std::views::values(curListener_)) {
            std::invoke(listener, node.param);
        }

        delete node.param;
    }

    co_return;
}

bool UEventSystem::IsQueueEmpty() const {
    std::shared_lock lock(sharedMutex_);
    return queue_.empty();
}

void UEventSystem::Dispatch(const EEvent event, IEventParam *parma) {
    const bool empty = IsQueueEmpty(); {
        std::scoped_lock lock(eventMutex_);
        queue_.emplace(event, parma);
    }

    if (empty)
        co_spawn(GetWorld().GetIOContext(), HandleEvent(), detached);
}

void UEventSystem::RegisterListener(const EEvent event, void *ptr, const EventListener &listener) {
    if (event == EEvent::UNAVAILABLE || ptr == nullptr)
        return;

    std::scoped_lock lock(listenerMutex_);
    if (!listenerMap_.contains(event))
        listenerMap_[event] = std::map<void *, EventListener>();

    listenerMap_[event][ptr] = listener;
}

void UEventSystem::RemoveListener(const EEvent event, void *ptr) {
    if (ptr == nullptr)
        return;

    std::scoped_lock lock(listenerMutex_);
    if (event == EEvent::UNAVAILABLE) {
        for (auto &val: std::views::values(listenerMap_)) {
            val.erase(ptr);
        }
    } else {
        if (const auto iter = listenerMap_.find(event); iter != listenerMap_.end()) {
            iter->second.erase(ptr);
        }
    }
}
