#pragma once

#include "../system/manager/IManager.h"
#include "CacheNode.h"

#include <mutex>
#include <shared_mutex>

class PlayerCache final : public base::IManager {

    std::unordered_map<uint64_t, CacheNode> cacheMap_;
    std::mutex blockMutex_;
    mutable std::shared_mutex sharedMutex_;

public:
    explicit PlayerCache(asio::io_context &ctx);
    ~PlayerCache() override;

    void OnTick(ATimePoint now) override;

    void SyncCache(CacheNode node);
    awaitable<std::optional<CacheNode>> FindCacheNode(uint64_t pid) const;
};
