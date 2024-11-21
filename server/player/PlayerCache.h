#pragma once

#include "CacheNode.h"

#include <system/manager/Manager.h>
#include <map>
#include <mutex>
#include <shared_mutex>

class UPlayerCache final : public IManager {

    std::map<uint64_t, FCacheNode> mCacheMap;
    std::mutex mBlockMutex;
    mutable std::shared_mutex mSharedMutex;

public:
    explicit UPlayerCache(FContextNode &ctx);
    ~UPlayerCache() override;

    [[nodiscard]] constexpr const char *GetManagerName() const override {
        return "UPlayerCache";
    }

    void OnTick(ATimePoint now) override;

    void SyncCache(FCacheNode node);
    awaitable<std::optional<FCacheNode>> FindCacheNode(uint64_t pid) const;
};
