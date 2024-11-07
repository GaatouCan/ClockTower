#include "MultiContextPool.h"

#include <spdlog/spdlog.h>


UMultiContextPool::UMultiContextPool()
    : mNextIndex(0) {
}

UMultiContextPool::~UMultiContextPool() {
    mWorkVec.clear();

    for (auto &thread: mThreadVec) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

void UMultiContextPool::Start(const size_t num) {
    mNodeVec = std::vector<FContextNode>(num);
    for (auto &node: mNodeVec) {
        mWorkVec.emplace_back(node.ctx);
        mThreadVec.emplace_back([this, &node] {
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
    if (mNodeVec.empty())
        throw std::runtime_error("No context node available");

    auto &res = mNodeVec[mNextIndex++];
    mNextIndex = mNextIndex % mNodeVec.size();
    return res;
}
