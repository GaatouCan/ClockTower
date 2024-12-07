#include "ManagerSystem.h"

#include <spdlog/spdlog.h>
#include <ranges>


UManagerSystem::UManagerSystem()
    : mManagerLoader(nullptr),
      mTickTimer(mContextNode.ctx) {
}

UManagerSystem::~UManagerSystem() {
    if (mManagerThread.joinable())
        mManagerThread.join();

    mTickTimer.cancel();

    if (!mContextNode.ctx.stopped())
        mContextNode.ctx.stop();

    for (const auto mgr: std::views::values(mManagerMap))
        delete mgr;
}

void UManagerSystem::Init() {
    if (mManagerLoader) {
        std::invoke(mManagerLoader, this);
    }

    mManagerThread = std::thread([this] {
        mContextNode.tid = std::this_thread::get_id();
        asio::signal_set signals(mContextNode.ctx, SIGINT, SIGTERM);
        signals.async_wait([&](auto, auto) {
            mContextNode.ctx.stop();
        });

        mContextNode.ctx.run();
    });

    co_spawn(mContextNode.ctx, [this]() mutable -> awaitable<void> {
        try {
            ATimePoint point = NowTimePoint();
            point = std::chrono::floor<std::chrono::seconds>(point);
            auto day = std::chrono::floor<std::chrono::days>(point).time_since_epoch().count();

            while (!mContextNode.ctx.stopped()) {
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
    return mContextNode.tid;
}

bool UManagerSystem::InManagerThread() const {
    return mContextNode.tid == std::this_thread::get_id();
}
