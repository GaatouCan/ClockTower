#include "ChatRoom.h"

UChatRoom::UChatRoom(const uint64_t roomId, const uint64_t ownerId)
    : mRoomId(roomId),
      mOwnerId(ownerId) {
}

UChatRoom::~UChatRoom() {
}

uint64_t UChatRoom::GetRoomID() const {
    return mRoomId;
}
