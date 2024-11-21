#pragma once

#include <common.h>
#include <memory>
#include <set>
#include <asio.hpp>

using asio::awaitable;

class UPlayer;


class UChatRoom final : public std::enable_shared_from_this<UChatRoom> {

    uint64_t mRoomId;
    uint64_t mLeaderId;

    std::set<uint64_t> mMemberSet;

    ATimePoint mCreateTime;
    ATimePoint mLastUpdateTime;

public:
    UChatRoom() = delete;

    UChatRoom(uint64_t roomId, uint64_t leaderId);
    ~UChatRoom();

    uint64_t GetRoomID() const;

    awaitable<void> SendAllRoomInfo(const std::shared_ptr<UPlayer> &plr = nullptr) const;
};
