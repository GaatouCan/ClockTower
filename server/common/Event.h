#pragma once

#define DISPATCH_EVENT(event, param) \
    if (const auto sys = GetSystem<UEventSystem>(); sys != nullptr) { \
        sys->Dispatch(static_cast<uint32_t>(EEvent::event), param); \
    } else { \
        spdlog::critical("{} - Failed to get event system.", __FUNCTION__); \
        delete param; \
        GetWorld().Shutdown(); \
    }

enum class EEvent : unsigned int {
    UNAVAILABLE,
    PLAYER_LOGIN,
    PLAYER_LOGOUT
};
