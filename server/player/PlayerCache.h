#pragma once

#include "../system/manager/Manager.h"
#include "CacheNode.h"

#include <mutex>
#include <shared_mutex>

class UPlayerCache final : public IManager {

    std::unordered_map<uint64_t, FCacheNode> mCacheMap;
    std::mutex mBlockMutex;
    mutable std::shared_mutex mSharedMutex;

public:
    explicit UPlayerCache(asio::io_context &ctx);
    ~UPlayerCache() override;

    void OnTick(ATimePoint now) override;

    void SyncCache(FCacheNode node);
    awaitable<std::optional<FCacheNode>> FindCacheNode(uint64_t pid) const;
};
