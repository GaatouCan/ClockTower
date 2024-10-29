#include "PlayerCache.h"
#include "../system/database/DatabaseSystem.h"
#include "../base/GameWorld.h"

PlayerCache::PlayerCache(asio::io_context &ctx) : IManager(ctx) {
}

PlayerCache::~PlayerCache() {

}

void PlayerCache::OnTick(ATimePoint now) {
    // TODO
}

void PlayerCache::SyncCache(CacheNode node) {
    std::unique_lock lock(blockMutex_);
    cacheMap_.insert_or_assign(node.pid, node);
}

awaitable<std::optional<CacheNode>> PlayerCache::FindCacheNode(const uint64_t pid) const {
    {
        std::shared_lock lock(sharedMutex_);
        if (const auto iter = cacheMap_.find(pid); iter != cacheMap_.end()) {
            co_return iter->second;
        }
    }
    const auto sys = GetSystem<base::DatabaseSystem>();
    if (sys == nullptr) {
        spdlog::error("{} - Failed to get DatabaseSystem", __func__);
        co_return std::nullopt;
    }

    co_await sys->AsyncPushTask([this, pid](mysqlx::Schema &schema) {
        // TODO
    }, asio::use_awaitable);

    std::shared_lock lock(sharedMutex_);
    if (const auto iter = cacheMap_.find(pid); iter != cacheMap_.end()) {
        co_return iter->second;
    }
    co_return std::nullopt;
}
