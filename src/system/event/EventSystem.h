#pragma once

#include "../../base/ISubSystem.h"
#include "IEventParam.h"
#include "../../common/event.h"

#include <memory>
#include <atomic>
#include <queue>
#include <mutex>
#include <map>


namespace base {
    class EventSystem final : public ISubSystem {
        SUB_SYSTEM_BODY(EventSystem)

        void Init() override;
        awaitable<void> HandleEvent();

        using EventListener = std::function<void(IEventParam *)>;

    public:
        void Dispatch(Event event, IEventParam *parma);

        template<typename TARGET, typename CALLABLE>
        void RegisterListener(const Event event, void *ptr, void *target, CALLABLE && func) {
            const EventListener listener = [target, func = std::forward<CALLABLE>(func)](IEventParam *param) {
                std::invoke(func, static_cast<TARGET *>(target), param);
            };
            RegisterListener(event, ptr, listener);
        }

        void RegisterListener(Event event, void *ptr, const EventListener &listener);

    private:

        struct EventNode {
            Event event;
            IEventParam *param;
        };

        std::unique_ptr<std::queue<EventNode>> curQueue_;
        std::unique_ptr<std::queue<EventNode>> waitQueue_;
        std::mutex eventMutex_;

        std::map<Event, std::map<void *, EventListener>> listenerMap_;
        std::mutex listenerMutex_;

        std::atomic_bool handling_;
    };
} // base
