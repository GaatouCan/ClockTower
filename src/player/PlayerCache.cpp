#include "PlayerCache.h"

PlayerCache::PlayerCache(asio::io_context &ctx) : IManager(ctx) {
}

PlayerCache::~PlayerCache() {

}

void PlayerCache::OnTick(TimePoint now) {
    // TODO
}

void PlayerCache::SyncCache(CacheNode *node) {
    if (node == nullptr)
        return;

    std::unique_lock lock(blockMutex_);
    cacheMap_.insert_or_assign(node->pid, node);
}

const CacheNode *PlayerCache::FindCacheNode(const uint64_t pid) const {
    std::shared_lock lock(sharedMutex_);
    if (const auto iter = cacheMap_.find(pid); iter != cacheMap_.end()) {
        return iter->second;
    }
    return nullptr;
}
