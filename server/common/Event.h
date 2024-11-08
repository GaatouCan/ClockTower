#pragma once

#define DispatchEvent(event, param) \
    Dispatch(static_cast<uint32_t>(EEvent::event), param)

enum class EEvent {
    UNAVAILABLE,
    PLAYER_LOGIN,
    PLAYER_LOGOUT
};
