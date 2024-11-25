#include "ChatRoom.h"

#include "../../../player/Player.h"
#include "../../../player/PlayerCache.h"
#include "system/manager/ManagerSystem.h"
#include "../../../common/ProtoType.h"
#include "ChatManager.h"

#include <manager/player/PlayerManager.h>
#include <chat.pb.h>


UChatRoom::UChatRoom(UChatManager *owner)
    : mOwner(owner),
      mRoomId(),
      mLeaderId(0),
      mChatIndex(1) {
}

UChatRoom::~UChatRoom() {
}

UChatRoom &UChatRoom::SetRoomID(const FGeneratedID roomId) {
    mRoomId = roomId;
    return *this;
}

UChatRoom &UChatRoom::SetLeaderID(const uint64_t leaderId) {
    mLeaderId = leaderId;
    return *this;
}

FGeneratedID UChatRoom::GetRoomID() const {
    return mRoomId;
}

uint64_t UChatRoom::GetLeaderID() const {
    return mLeaderId;
}


awaitable<void> UChatRoom::SendAllRoomInfo(const std::shared_ptr<UPlayer> &plr) const {
    const auto cacheMgr = GetManager<UPlayerCache>();
    if (cacheMgr == nullptr)
        co_return;

    const auto playerMgr = GetManager<UPlayerManager>();
    if (playerMgr == nullptr)
        co_return;

    Chat::SC_ChatRoomResponse response;

    response.set_roomid(mRoomId.ToString());
    response.set_leader(mLeaderId);
    response.set_reason(Chat::SC_ChatRoomResponse::NORMAL_SEND);

    for (const auto &memberId: mMemberSet) {
        auto node = co_await cacheMgr->FindCacheNode(memberId);
        if (!node.has_value())
            continue;

        auto cacheNode = node.value();
        const auto member = response.add_memberlist();
        member->set_pid(memberId);
        member->set_online(cacheNode.IsOnline());
    }

    if (plr != nullptr) {
        SEND_PACKAGE(plr, SC_ChatRoomResponse, response);
    } else {
        const auto pkg = dynamic_cast<FPackage *>(mOwner->BuildPackage());

        BUILD_PACKAGE(pkg, SC_ChatRoomResponse, response)
        playerMgr->SendToList(pkg, mMemberSet);
    }
}

awaitable<void> UChatRoom::UpdateMemberInfo(std::set<uint64_t> members,
                                            const std::vector<FCacheNode *> &cacheVec) const {
    const auto cacheMgr = GetManager<UPlayerCache>();
    if (cacheMgr == nullptr)
        co_return;

    const auto playerMgr = GetManager<UPlayerManager>();
    if (playerMgr == nullptr)
        co_return;

    Chat::SC_ChatRoomResponse response;

    response.set_roomid(mRoomId.ToString());
    response.set_leader(mLeaderId);
    response.set_reason(Chat::SC_ChatRoomResponse::MEMBER_UPDATE);

    for (const auto &cache: cacheVec) {
        if (cache == nullptr)
            continue;

        if (!mMemberSet.contains(cache->pid))
            continue;

        const auto member = response.add_memberlist();
        member->set_pid(cache->pid);
        member->set_online(cache->IsOnline());

        members.erase(cache->pid);
    }

    for (const uint64_t &memberId: members) {
        auto node = co_await cacheMgr->FindCacheNode(memberId);
        if (!node.has_value())
            continue;

        auto cacheNode = node.value();
        const auto member = response.add_memberlist();
        member->set_pid(memberId);
        member->set_online(cacheNode.IsOnline());
    }

    const auto pkg = dynamic_cast<FPackage *>(mOwner->BuildPackage());

    BUILD_PACKAGE(pkg, SC_ChatRoomResponse, response)
    playerMgr->SendToList(pkg, mMemberSet);
}

void UChatRoom::OnChat(const std::shared_ptr<UPlayer> &plr, const FChatContent &content) {
    if (const auto now = std::chrono::steady_clock::now(); mLastUpdateTime != now) {
        mLastUpdateTime = now;
        mChatIndex = 1;
    }
}
