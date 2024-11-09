#pragma once

#define DispatchEvent(event, param) \
    if (const auto sys = GetSystem<UEventSystem>(); sys != nullptr) { \
        sys->Dispatch(static_cast<uint32_t>(EEvent::event), param); \
    } else { \
        spdlog::critical("{} - Failed to get event system.", __func__); \
        delete param; \
        GetWorld().Shutdown(); \
    }

enum class EEvent {
    UNAVAILABLE,
    PLAYER_LOGIN,
    PLAYER_LOGOUT
};
