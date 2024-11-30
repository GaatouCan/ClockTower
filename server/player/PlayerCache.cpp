#include "PlayerCache.h"

#include <GameWorld.h>
#include <system/database/DatabaseSystem.h>


UPlayerCache::UPlayerCache(FContextNode &ctx) : IManager(ctx) {
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

    std::shared_lock lock(mSharedMutex);
    if (const auto iter = mCacheMap.find(pid); iter != mCacheMap.end()) {
        co_return iter->second;
    }

    auto result = co_await sys->AsyncSelect("player_cache", fmt::format("pid = {}", pid), asio::use_awaitable);

    if (result.has_value()) {
        // TODO: 根据数据库构造返回
    }

    co_return std::nullopt;
}
