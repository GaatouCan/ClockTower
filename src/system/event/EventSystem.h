#pragma once

#include "../../base/SubSystem.h"
#include "EventParam.h"
#include "../../common/event.h"

#include <memory>
#include <queue>
#include <mutex>
#include <shared_mutex>
#include <map>
#include <spdlog/spdlog.h>


class UEventSystem final : public ISubSystem {

    SUB_SYSTEM_BODY(EventSystem)

    ~UEventSystem() override;

    void Init() override;

    awaitable<void> HandleEvent();

public:
    [[nodiscard]] bool IsQueueEmpty() const;

    void Dispatch(EEvent event, IEventParam *parma);

    template<typename TARGET, typename CALLABLE>
    void RegisterListenerT(const EEvent event, void *ptr, void *target, CALLABLE &&func) {
        if (event == EEvent::UNAVAILABLE || ptr == nullptr || target == nullptr)
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

    void RegisterListener(EEvent event, void *ptr, const EventListener &listener);

    void RemoveListener(EEvent event, void *ptr);

private:
    struct EventNode {
        EEvent event = EEvent::UNAVAILABLE;
        IEventParam *param = nullptr;
    };

    std::unique_ptr<std::queue<EventNode> > queue_;
    std::mutex eventMutex_;
    mutable std::shared_mutex sharedMutex_;

    std::map<EEvent, std::map<void *, EventListener> > listenerMap_;
    std::map<void *, EventListener> curListener_;
    std::mutex listenerMutex_;
};
