#pragma once

#include "common.h"
#include "PackagePool.h"

/**
 * 每个IO线程的数据节点
 */
struct FContextNode {
    UPackagePool pool;
    asio::io_context ctx;
    AThreadID tid;
};

/**
 * 多线程池
 * 每个线程单独一个io_context和一个数据包池
 */
class UMultiContextPool final {

    std::vector<FContextNode> mNodeVec;
    std::vector<asio::io_context::work> mWorkVec;
    std::vector<std::thread> mThreadVec;

    std::atomic_size_t mNextIndex;

public:
    UMultiContextPool();
    ~UMultiContextPool();

    DISABLE_COPY_MOVE(UMultiContextPool)

    void Start(size_t num = std::thread::hardware_concurrency());

    FContextNode &NextContextNode();
};
