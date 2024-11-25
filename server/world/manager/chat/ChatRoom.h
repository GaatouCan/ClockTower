#pragma once

#include <common.h>
#include <memory>
#include <set>
#include <vector>
#include <asio.hpp>

#include <GeneratedID.h>

using asio::awaitable;

class UPlayer;
struct FCacheNode;

class UChatRoom final : public std::enable_shared_from_this<UChatRoom> {

    class UChatManager *mOwner;

    FGeneratedID mRoomId;
    uint64_t mLeaderId;

    std::set<uint64_t> mMemberSet;

    ATimePoint mCreateTime;
    ATimePoint mLastUpdateTime;

public:
    UChatRoom() = delete;

    explicit UChatRoom(UChatManager *owner);
    ~UChatRoom();

    UChatRoom &SetRoomID(FGeneratedID roomId);
    [[nodiscard]] FGeneratedID GetRoomID() const;

    UChatRoom &SetLeaderID(uint64_t leaderId);
    [[nodiscard]] uint64_t GetLeaderID() const;

    awaitable<void> SendAllRoomInfo(const std::shared_ptr<UPlayer> &plr = nullptr) const;

    awaitable<void> UpdateMemberInfo(std::set<uint64_t> members, const std::vector<FCacheNode *> &cacheVec = {}) const;
};