#include "MultiContextPool.h"

namespace base {
    MultiContextPool::MultiContextPool()
        : nextIndex_(0) {
    }

    MultiContextPool::~MultiContextPool() {
        workVec_.clear();

        for (auto &thread : threadVec_) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

    void MultiContextPool::start(const size_t num) {
        nodeVec_ = std::vector<ContextNode>(num);
        for (auto &node : nodeVec_) {
            workVec_.emplace_back(node.ctx);
            threadVec_.emplace_back([this, &node] {
                asio::signal_set signals(node.ctx, SIGINT, SIGTERM);
                signals.async_wait([&](auto, auto) {
                    node.ctx.stop();
                });
                node.ctx.run();
            });
        }
    }

    ContextNode &MultiContextPool::nextContextNode() {
        auto &res = nodeVec_[nextIndex_++];
        nextIndex_ = nextIndex_ % nodeVec_.size();
        return res;
    }
} // base
