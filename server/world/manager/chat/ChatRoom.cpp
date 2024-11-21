#include "ChatRoom.h"

#include "../../../player/PlayerCache.h"
#include "system/manager/ManagerSystem.h"

#include <chat.pb.h>

UChatRoom::UChatRoom(const uint64_t roomId, const uint64_t leaderId)
    : mRoomId(roomId),
      mLeaderId(leaderId) {
}

UChatRoom::~UChatRoom() {
}

uint64_t UChatRoom::GetRoomID() const {
    return mRoomId;
}

awaitable<void> UChatRoom::SendAllRoomInfo(const std::shared_ptr<UPlayer> &plr) const {
    const auto cacheMgr = GetManager<UPlayerCache>();
    if (cacheMgr == nullptr)
        co_return;

    Chat::SC_ChatRoomResponse response;

    response.set_roomid(mRoomId);
    response.set_leader(mLeaderId);

    for (const auto memberId : mMemberSet) {
        auto node = co_await cacheMgr->FindCacheNode(memberId);
        if (!node.has_value())
            continue;

        auto cacheNode = node.value();
        const auto member = response.add_memberlist();
        member->set_pid(memberId);
        member->set_online(cacheNode.IsOnline());
    }
}
