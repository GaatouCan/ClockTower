#pragma once

#include "../system/manager/IManager.h"
#include "CacheNode.h"

class PlayerCache final : public base::IManager {

    std::unordered_map<uint64_t, CacheNode> cacheMap_;

public:
    explicit PlayerCache(asio::io_context &ctx);
    ~PlayerCache() override;
};
