#include "ChatRoom.h"

#include "../../../player/Player.h"
#include "../../../player/PlayerCache.h"
#include "system/manager/ManagerSystem.h"
#include "../../../common/ProtoType.h"

#include <manager/player/PlayerManager.h>
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

    const auto playerMgr = GetManager<UPlayerManager>();
    if (playerMgr == nullptr)
        co_return;

    Chat::SC_ChatRoomResponse response;

    response.set_roomid(mRoomId);
    response.set_leader(mLeaderId);

    std::set<uint64_t> targetPlayerSet;

    for (const auto memberId : mMemberSet) {
        auto node = co_await cacheMgr->FindCacheNode(memberId);
        if (!node.has_value())
            continue;

        auto cacheNode = node.value();
        const auto member = response.add_memberlist();
        member->set_pid(memberId);
        member->set_online(cacheNode.IsOnline());

        if (cacheNode.IsOnline()) {
            targetPlayerSet.insert(memberId);
        }
    }

    if (plr != nullptr) {
        plr->SendPackage(SC_ChatRoomResponse, response);
    }
    else {
        // for (const auto memberId : targetPlayerSet) {
        //     if (const auto member = std::dynamic_pointer_cast<UPlayer>(playerMgr->FindPlayer(memberId)); member != nullptr && member->IsOnline()) {
        //         member->SendPackage(SC_ChatRoomResponse, response);
        //     }
        // }
    }
}
