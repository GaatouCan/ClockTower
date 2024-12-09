#include "ChatRoom.h"

#include "../../../player/Player.h"
#include "../../../player/PlayerManager.h"
#include "system/manager/manager_system.h"
#include "../../../common/ProtoType.h"
#include "ChatManager.h"

#include <chat.pb.h>

UChatRoom::UChatRoom(UChatManager *owner)
    : mOwner(owner),
      mRoomId(),
      mLeaderId(0),
      mChatIndex(1) {
}

UChatRoom::~UChatRoom() {
}

UChatRoom &UChatRoom::SetRoomID(const FUniqueID roomId) {
    mRoomId = roomId;
    return *this;
}

UChatRoom &UChatRoom::SetLeaderID(const FPlayerID &leaderId) {
    mLeaderId = leaderId;
    return *this;
}

FUniqueID UChatRoom::GetRoomID() const {
    return mRoomId;
}

FPlayerID UChatRoom::GetLeaderID() const {
    return mLeaderId;
}


awaitable<void> UChatRoom::SendAllRoomInfo(const std::shared_ptr<UPlayer> &plr) const {
    const auto playerMgr = GetManager<UPlayerManager>();
    if (playerMgr == nullptr)
        co_return;

    Chat::SC_ChatRoomResponse response;

    response.set_roomid(mRoomId.ToString());
    response.set_leader(mLeaderId.ToUInt64());
    response.set_reason(Chat::SC_ChatRoomResponse::NORMAL_SEND);

    for (const auto &memberId: mMemberSet) {
        auto node = co_await playerMgr->FindCacheNode(memberId);
        if (!node.has_value())
            continue;

        auto cacheNode = node.value();
        const auto member = response.add_memberlist();
        member->set_pid(memberId.ToUInt64());
        member->set_online(cacheNode.IsOnline());
    }

    if (plr != nullptr) {
        SEND_PACKAGE(plr, SC_ChatRoomResponse, response)
    } else {
        const auto pkg = dynamic_cast<FPackage *>(mOwner->BuildPackage());

        BUILD_PACKAGE(pkg, SC_ChatRoomResponse, response)
        playerMgr->SendToList(pkg, mMemberSet);
    }
}

awaitable<void> UChatRoom::UpdateMemberInfo(std::set<FPlayerID> members, const std::vector<FCacheNode *> &cacheVec) const {
    const auto playerMgr = GetManager<UPlayerManager>();
    if (playerMgr == nullptr)
        co_return;

    Chat::SC_ChatRoomResponse response;

    response.set_roomid(mRoomId.ToString());
    response.set_leader(mLeaderId.ToUInt64());
    response.set_reason(Chat::SC_ChatRoomResponse::MEMBER_UPDATE);

    for (const auto &cache: cacheVec) {
        if (cache == nullptr)
            continue;

        if (!mMemberSet.contains(cache->pid))
            continue;

        const auto member = response.add_memberlist();
        member->set_pid(cache->pid.ToUInt64());
        member->set_online(cache->IsOnline());

        members.erase(cache->pid);
    }

    for (const auto &memberId: members) {
        auto node = co_await playerMgr->FindCacheNode(memberId);
        if (!node.has_value())
            continue;

        auto cacheNode = node.value();
        const auto member = response.add_memberlist();
        member->set_pid(memberId.ToUInt64());
        member->set_online(cacheNode.IsOnline());
    }

    const auto pkg = dynamic_cast<FPackage *>(mOwner->BuildPackage());

    BUILD_PACKAGE(pkg, SC_ChatRoomResponse, response)
    playerMgr->SendToList(pkg, mMemberSet);
}

void UChatRoom::OnChat(const std::shared_ptr<UPlayer> &plr, const FChatContent &content) {
    Chat::SC_ChatToRoomResponse response;

    response.set_roomid(mRoomId.ToString());
    response.set_clienttime(content.clientTime);
    response.set_clientindex(content.clientIndex);

    if (!mMemberSet.contains(plr->GetPlayerID())) {
        response.set_reason(Chat::SC_ChatToRoomResponse::NOT_ROOM_MEMBER);
        SEND_PACKAGE(plr, SC_ChatToRoomResponse, response)
        return;
    }

    if (const auto now = NowTimePoint(); mLastUpdateTime != now) {
        mLastUpdateTime = now;
        mChatIndex = 1;
    }

    response.set_servertime(ToUnixTime(mLastUpdateTime));
    response.set_serverindex(mChatIndex);

    const auto playerMgr = GetManager<UPlayerManager>();
    if (playerMgr == nullptr) {
        response.set_reason(Chat::SC_ChatToRoomResponse::IN_LIMITED);
        SEND_PACKAGE(plr, SC_ChatRoomResponse, response)
        return;
    }

    response.set_reason(Chat::SC_ChatToRoomResponse::SUCCESS);
    SEND_PACKAGE(plr, SC_ChatToRoomResponse, response)

    Chat::SC_OnChatRoomResponse msg;

    msg.set_roomid(mRoomId.ToString());
    msg.set_sender(plr->GetFullID());
    msg.set_content(content.content);
    msg.set_servertime(ToUnixTime(mLastUpdateTime));
    msg.set_serverindex(mChatIndex++);
    msg.set_reftime(content.refTime);
    msg.set_refindex(content.refIndex);

    const auto mPkg = dynamic_cast<FPackage *>(mOwner->BuildPackage());
    BUILD_PACKAGE(mPkg, SC_OnChatRoomResponse, msg)
    playerMgr->SendToList(mPkg, mMemberSet);
}
