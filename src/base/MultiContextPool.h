#pragma once

#include "../common/common.h"
#include "PackagePool.h"

namespace base {
    /**
     * 每个IO线程的数据节点
     */
    struct ContextNode {
        PackagePool pool;
        asio::io_context ctx;
        ThreadID tid;
    };

    /**
     * 多线程池
     * 每个线程单独一个io_context和一个数据包池
     */
    class MultiContextPool final {

        std::vector<ContextNode> nodeVec_;
        std::vector<asio::io_context::work> workVec_;
        std::vector<std::thread> threadVec_;

        std::atomic_size_t nextIndex_;

    public:
        MultiContextPool();
        ~MultiContextPool();

        DISABLE_COPY_MOVE(MultiContextPool)

        void Start(size_t num = std::thread::hardware_concurrency());

        ContextNode &NextContextNode();
    };
} // base
