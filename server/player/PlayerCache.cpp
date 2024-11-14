#include "PlayerCache.h"

#include "../base/GameWorld.h"
#include "../system/database/DatabaseSystem.h"


UPlayerCache::UPlayerCache(asio::io_context &ctx) : IManager(ctx) {
}

UPlayerCache::~UPlayerCache() {

}

void UPlayerCache::OnTick(ATimePoint now) {
    // TODO
}

void UPlayerCache::SyncCache(FCacheNode node) {
    std::unique_lock lock(mBlockMutex);
    mCacheMap.insert_or_assign(node.pid, node);
}

awaitable<std::optional<FCacheNode>> UPlayerCache::FindCacheNode(const uint64_t pid) const {
    {
        std::shared_lock lock(mSharedMutex);
        if (const auto iter = mCacheMap.find(pid); iter != mCacheMap.end()) {
            co_return iter->second;
        }
    }
    const auto sys = GetSystem<UDatabaseSystem>();
    if (sys == nullptr) {
        spdlog::error("{} - Failed to get DatabaseSystem", __FUNCTION__);
        co_return std::nullopt;
    }

    co_await sys->AsyncPushTask([this, pid](mysqlx::Schema &schema) {
        // TODO
    }, asio::use_awaitable);

    std::shared_lock lock(mSharedMutex);
    if (const auto iter = mCacheMap.find(pid); iter != mCacheMap.end()) {
        co_return iter->second;
    }
    co_return std::nullopt;
}
