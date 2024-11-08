#include "ManagerSystem.h"

#include <spdlog/spdlog.h>

UManagerSystem::UManagerSystem()
    : mLoader(nullptr),
      mTickTimer(mIOContext) {
}

UManagerSystem::~UManagerSystem() {
    if (mManagerThread.joinable())
        mManagerThread.join();

    mTickTimer.cancel();

    if (!mIOContext.stopped())
        mIOContext.stop();

    for (const auto mgr: std::views::values(mManagerMap))
        delete mgr;
}

void UManagerSystem::Init() {
    bCouldLoad = true;

    if (mLoader) {
        std::invoke(mLoader, this);
    }

    mManagerThread = std::thread([this] {
        mManagerThreadId = std::this_thread::get_id();
        asio::signal_set signals(mIOContext, SIGINT, SIGTERM);
        signals.async_wait([&](auto, auto) {
            mIOContext.stop();
        });

        for (const auto mgr: std::views::values(mManagerMap))
            mgr->SetThreadID(mManagerThreadId);

        mIOContext.run();
    });

    co_spawn(mIOContext, [this]() mutable -> awaitable<void> {
        try {
            ATimePoint point = std::chrono::steady_clock::now();
            point = std::chrono::floor<std::chrono::seconds>(point);

            while (!mIOContext.stopped()) {
                point += std::chrono::seconds(1);
                mTickTimer.expires_at(point);
                co_await mTickTimer.async_wait();

                for (const auto mgr: std::views::values(mManagerMap))
                    mgr->OnTick(point);
            }
        } catch (std::exception &e) {
            spdlog::warn("{}", e.what());
        }
    }, detached);

    bCouldLoad = false;
}

void UManagerSystem::SetManagerLoader(const std::function<void(UManagerSystem *)> &loader) {
    mLoader = loader;
}

AThreadID UManagerSystem::GetThreadID() const {
    return mManagerThreadId;
}

bool UManagerSystem::InManagerThread() const {
    return mManagerThreadId == std::this_thread::get_id();
}

bool UManagerSystem::IsTimeToLoadManager() const {
    return bCouldLoad;
}
