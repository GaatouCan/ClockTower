#pragma once

#include <common.h>
#include <memory>
#include <set>
#include <asio.hpp>

using asio::awaitable;

class UPlayer;

class UChatRoom final : public std::enable_shared_from_this<UChatRoom> {

    class UChatManager *mOwner;

    uint64_t mRoomId;
    uint64_t mLeaderId;

    std::set<uint64_t> mMemberSet;

    ATimePoint mCreateTime;
    ATimePoint mLastUpdateTime;

public:
    UChatRoom() = delete;

    explicit UChatRoom(UChatManager *owner);
    ~UChatRoom();

    UChatRoom &SetRoomID(uint64_t roomId);
    [[nodiscard]] uint64_t GetRoomID() const;

    UChatRoom &SetLeaderID(uint64_t leaderId);
    [[nodiscard]] uint64_t GetLeaderID() const;

    awaitable<void> SendAllRoomInfo(const std::shared_ptr<UPlayer> &plr = nullptr) const;
};