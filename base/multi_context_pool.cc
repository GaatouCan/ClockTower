#include "multi_context_pool.h"

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

    for (size_t idx = 0; idx < num; ++idx) {
        mNodeVec[idx].mIndex = idx + 1;
    }

    for (auto &node: mNodeVec) {
        mWorkVec.emplace_back(node.mIOContext);
        mThreadVec.emplace_back([this, &node] {
            asio::signal_set signals(node.mIOContext, SIGINT, SIGTERM);
            signals.async_wait([&](auto, auto) {
                node.mIOContext.stop();
            });

            node.mThreadID = std::this_thread::get_id();
            node.mIOContext.run();
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
