#include "MultiContextPool.h"

#include <spdlog/spdlog.h>

namespace base {
    UMultiContextPool::UMultiContextPool()
        : nextIndex_(0) {
    }

    UMultiContextPool::~UMultiContextPool() {
        workVec_.clear();

        for (auto &thread : threadVec_) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

    void UMultiContextPool::Start(const size_t num) {
        nodeVec_ = std::vector<FContextNode>(num);
        for (auto &node : nodeVec_) {
            workVec_.emplace_back(node.ctx);
            threadVec_.emplace_back([this, &node] {
                asio::signal_set signals(node.ctx, SIGINT, SIGTERM);
                signals.async_wait([&](auto, auto) {
                    node.ctx.stop();
                });

                node.tid = std::this_thread::get_id();
                node.ctx.run();
            });
        }
        spdlog::info("MultiContextPool Started With {} Thread(s).", num);
    }

    FContextNode &UMultiContextPool::NextContextNode() {
        if (nodeVec_.empty())
            throw std::runtime_error("No context node available");

        auto &res = nodeVec_[nextIndex_++];
        nextIndex_ = nextIndex_ % nodeVec_.size();
        return res;
    }
} // base
