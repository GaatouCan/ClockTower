#pragma once

#include "../../base/ISubSystem.h"
#include "IEventParam.h"
#include "../../common/event.h"

#include <memory>
#include <atomic>
#include <queue>
#include <mutex>


namespace base {
    class EventSystem final : public ISubSystem {
        SUB_SYSTEM_BODY(EventSystem)

        void Init() override;
        awaitable<void> HandleEvent();

    public:
        void Dispatch(Event event, IEventParam *parma);

    private:

        struct EventNode {
            Event event;
            IEventParam *param;
        };

        std::unique_ptr<std::queue<EventNode>> curQueue_;
        std::unique_ptr<std::queue<EventNode>> waitQueue_;
        std::mutex eventMutex_;

        std::atomic_bool handling_;
    };
} // base
