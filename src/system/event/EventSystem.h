#pragma once

#include "../../base/ISubSystem.h"
#include "IEventParam.h"
#include "../../common/event.h"

#include <memory>
#include <atomic>
#include <queue>
#include <mutex>
#include <shared_mutex>
#include <map>
#include <spdlog/spdlog.h>


namespace base {
    class EventSystem final : public ISubSystem {
        SUB_SYSTEM_BODY(EventSystem)

        void Init() override;
        awaitable<void> HandleEvent();

        using EventListener = std::function<void(IEventParam *)>;

    public:
        [[nodiscard]] bool IsQueueEmpty() const;

        void Dispatch(Event event, IEventParam *parma);

        template<typename TARGET, typename CALLABLE>
        void RegisterListenerT(const Event event, void *ptr, void *target, CALLABLE && func) {
            if (event == Event::UNAVAILABLE || ptr == nullptr || target == nullptr)
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

        void RegisterListener(Event event, void *ptr, const EventListener &listener);

        void RemoveListener(Event event, void *ptr);

    private:

        struct EventNode {
            Event event = Event::UNAVAILABLE;
            IEventParam *param = nullptr;
        };

        std::unique_ptr<std::queue<EventNode>> queue_;
        std::mutex eventMutex_;
        mutable std::shared_mutex sharedMutex_;

        std::map<Event, std::map<void *, EventListener>> listenerMap_;
        std::map<void *, EventListener> curListener_;
        std::mutex listenerMutex_;
    };
} // base
