#pragma once

#include "../common/common.h"
#include "PackagePool.h"

namespace base {

    struct ContextNode {
        PackagePool pool;
        asio::io_context ctx;
    };

    class MultiContextPool final {

        std::vector<ContextNode> nodeVec_;
        std::vector<asio::io_context::work> workVec_;
        std::vector<std::thread> threadVec_;

        std::atomic_size_t nextIndex_;

    public:
        MultiContextPool();
        ~MultiContextPool();

        DISABLE_COPY_MOVE(MultiContextPool)

        void start(size_t num = std::thread::hardware_concurrency());

        ContextNode &nextContextNode();
    };
} // base