#pragma once

#include <common.h>
#include <memory>
#include <vector>


class UChatRoom final : public std::enable_shared_from_this<UChatRoom> {

    uint64_t mRoomId;
    uint64_t mOwnerId;

    std::vector<uint64_t> mMemberList;

    ATimePoint mCreateTime;
    ATimePoint mLastUpdateTime;

public:
    UChatRoom(uint64_t roomId, uint64_t ownerId);
    ~UChatRoom();

    uint64_t GetRoomID() const;
};
