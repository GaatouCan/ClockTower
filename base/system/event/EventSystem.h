#pragma once

#include "../../sub_system.h"
#include "EventParam.h"

#include <queue>
#include <mutex>
#include <shared_mutex>
#include <map>
#include <spdlog/spdlog.h>


class UEventSystem final : public ISubSystem {

    struct FEventNode {
        uint32_t event = 0;
        IEventParam *param = nullptr;
    };

    std::queue<FEventNode> mEventQueue;
    std::mutex mEventMutex;
    mutable std::shared_mutex mSharedMutex;

    std::map<uint32_t, std::map<void *, EventListener> > mListenerMap;
    std::map<void *, EventListener> mCurrentListener;
    std::mutex mListenerMutex;

public:
    ~UEventSystem() override;

    void Init() override;

    [[nodiscard]] constexpr const char * GetSystemName() const override {
        return "UEventSystem";
    }

    void Dispatch(uint32_t event, IEventParam *parma);

    template<typename TARGET, typename CALLABLE>
    void RegisterListenerT(const uint32_t event, void *ptr, void *target, CALLABLE &&func) {
        if (ptr == nullptr || target == nullptr)
            return;

        this->RegisterListener(event, ptr, [target, func = std::forward<CALLABLE>(func)](IEventParam *param) {
            try {
                if (target != nullptr) {
                    std::invoke(func, static_cast<TARGET *>(target), param);
                }
            } catch (std::exception &e) {
                spdlog::warn("Event Listener - {}", e.what());
            }
        });
    }

    void RegisterListener(uint32_t event, void *ptr, const EventListener &listener);

    void RemoveListener(uint32_t event, void *ptr);

private:
    awaitable<void> HandleEvent();

    [[nodiscard]] bool IsQueueEmpty() const;
};
