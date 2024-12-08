﻿#pragma once

#include "context_node.h"

/**
 * 多线程池
 * 每个线程单独一个io_context和一个数据包池
 */
class UMultiContextPool final {

    std::vector<FContextNode> mNodeVec;
    std::vector<AIOContext::work> mWorkVec;
    std::vector<std::thread> mThreadVec;

    std::atomic_size_t mNextIndex;

public:
    UMultiContextPool();
    ~UMultiContextPool();

    DISABLE_COPY_MOVE(UMultiContextPool)

    void Start(size_t num = std::thread::hardware_concurrency());

    FContextNode &NextContextNode();
};
