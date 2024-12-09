#include "ManagerSystem.h"

#include <spdlog/spdlog.h>
#include <ranges>


UManagerSystem::UManagerSystem()
    : mManagerLoader(nullptr),
      mTickTimer(mContextNode.mIOContext) {
}

UManagerSystem::~UManagerSystem() {
    if (mManagerThread.joinable())
        mManagerThread.join();

    mTickTimer.cancel();

    if (!mContextNode.mIOContext.stopped())
        mContextNode.mIOContext.stop();

    for (const auto mgr: std::views::values(mManagerMap))
        delete mgr;
}

void UManagerSystem::Init() {
    if (mManagerLoader) {
        std::invoke(mManagerLoader, this);
    }

    mManagerThread = std::thread([this] {
        mContextNode.mThreadID = std::this_thread::get_id();
        asio::signal_set signals(mContextNode.mIOContext, SIGINT, SIGTERM);
        signals.async_wait([&](auto, auto) {
            mContextNode.mIOContext.stop();
        });

        mContextNode.mIOContext.run();
    });

    co_spawn(mContextNode.mIOContext, [this]() mutable -> awaitable<void> {
        try {
            ATimePoint point = NowTimePoint();
            point = std::chrono::floor<std::chrono::seconds>(point);
            auto day = std::chrono::floor<std::chrono::days>(point).time_since_epoch().count();

            while (!mContextNode.mIOContext.stopped()) {
                point += std::chrono::seconds(1);
                mTickTimer.expires_at(point);
                co_await mTickTimer.async_wait();

                for (const auto mgr: std::views::values(mManagerMap))
                    mgr->OnTick(point);

                if (const auto today = std::chrono::floor<std::chrono::days>(point).time_since_epoch().count(); today != day) {
                    day = today;
                    for (const auto mgr: std::views::values(mManagerMap))
                        mgr->OnDayChange();
                }
            }
        } catch (std::exception &e) {
            spdlog::warn("{}", e.what());
        }
    }, detached);
}

void UManagerSystem::SetManagerLoader(const std::function<void(UManagerSystem *)> &loader) {
    mManagerLoader = loader;
}

AThreadID UManagerSystem::GetThreadID() const {
    return mContextNode.mThreadID;
}

bool UManagerSystem::InManagerThread() const {
    return mContextNode.mThreadID == std::this_thread::get_id();
}
