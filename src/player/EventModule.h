#pragma once

#include "../system/event/IEventParam.h"
#include "../common/event.h"

#include <memory>
#include <queue>
#include <mutex>
#include <shared_mutex>
#include <map>
#include <spdlog/spdlog.h>


class EventModule final {

    class Player *owner_;

    struct EventNode {
        Event event = Event::UNAVAILABLE;
        base::IEventParam *param = nullptr;
    };

    std::unique_ptr<std::queue<EventNode>> queue_;
    std::mutex eventMutex_;
    mutable std::shared_mutex sharedMutex_;

    std::map<Event, std::map<void *, EventListener>> listenerMap_;
    std::map<void *, EventListener> curListener_;
    std::mutex listenerMutex_;

public:
    EventModule() = delete;

    explicit EventModule(Player *plr);
    ~EventModule();

    [[nodiscard]] Player *GetOwner() const;

    template<typename TARGET, typename CALLABLE>
        void RegisterListenerT(const Event event, void *ptr, void *target, CALLABLE && func) {
        if (event == Event::UNAVAILABLE || ptr == nullptr || target == nullptr)
            return;

        this->RegisterListener(event, ptr, [target, func = std::forward<CALLABLE>(func)](base::IEventParam *param) {
            try {
                if (target != nullptr) {
                    std::invoke(func, static_cast<TARGET *>(target), param);
                }
            } catch (std::exception &e) {
                spdlog::warn("Player Event Module - {}", e.what());
            }
        });
    }

    void RegisterListener(Event event, void *ptr, const EventListener &listener);

    void RemoveListener(Event event, void *ptr);
};
