﻿#include "event_system.h"
#include "../../game_world.h"

#include <ranges>

UEventSystem::~UEventSystem() {
    mListenerMap.clear();

    while (!mEventQueue.empty()) {
        auto &[event, param] = mEventQueue.front();
        mEventQueue.pop();
        delete param;
    }
}

void UEventSystem::Init() {

}

awaitable<void> UEventSystem::HandleEvent() {
    while (!IsQueueEmpty()) {
        FEventNode node; {
            std::scoped_lock lock(mEventMutex);
            node = mEventQueue.front();
            mEventQueue.pop();
        }

        if (node.event == 0) {
            delete node.param;
            continue;
        }

        mCurrentListener.clear();

        // 拷贝一份map 减少锁的范围 可以在调用事件处理函数时修改注册map
        {
            std::scoped_lock lock(mListenerMutex);
            if (const auto iter = mListenerMap.find(node.event); iter != mListenerMap.end()) {
                mCurrentListener = iter->second;
            }
        }

        for (const auto &listener: std::views::values(mCurrentListener)) {
            std::invoke(listener, node.param);
        }

        delete node.param;
    }

    co_return;
}

bool UEventSystem::IsQueueEmpty() const {
    std::shared_lock lock(mSharedMutex);
    return mEventQueue.empty();
}

void UEventSystem::Dispatch(const uint32_t event, IEventParam *parma) {
    const bool empty = IsQueueEmpty(); {
        std::scoped_lock lock(mEventMutex);
        mEventQueue.emplace(event, parma);
    }

    if (empty)
        co_spawn(GetWorld().GetIOContext(), HandleEvent(), detached);
}

void UEventSystem::RegisterListener(const uint32_t event, void *ptr, const EventListener &listener) {
    if (event == 0 || ptr == nullptr)
        return;

    std::scoped_lock lock(mListenerMutex);
    if (!mListenerMap.contains(event))
        mListenerMap[event] = std::map<void *, EventListener>();

    mListenerMap[event][ptr] = listener;
}

void UEventSystem::RemoveListener(const uint32_t event, void *ptr) {
    if (ptr == nullptr)
        return;

    std::scoped_lock lock(mListenerMutex);
    if (event == 0) {
        for (auto &val: std::views::values(mListenerMap)) {
            val.erase(ptr);
        }
    } else {
        if (const auto iter = mListenerMap.find(event); iter != mListenerMap.end()) {
            iter->second.erase(ptr);
        }
    }
}
