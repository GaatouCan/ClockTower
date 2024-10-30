#include "ManagerSystem.h"

#include <spdlog/spdlog.h>


UManagerSystem::UManagerSystem()
    : timer_(ctx_) {
}

UManagerSystem::~UManagerSystem() {
    if (mgrThread_.joinable())
        mgrThread_.join();

    timer_.cancel();

    if (!ctx_.stopped())
        ctx_.stop();

    for (const auto mgr: std::views::values(mgrMap_))
        delete mgr;
}

void UManagerSystem::Init() {
    mgrThread_ = std::thread([this] {
        tid_ = std::this_thread::get_id();
        asio::signal_set signals(ctx_, SIGINT, SIGTERM);
        signals.async_wait([&](auto, auto) {
            ctx_.stop();
        });

        for (const auto mgr: std::views::values(mgrMap_))
            mgr->SetThreadID(tid_);

        ctx_.run();
    });

    co_spawn(ctx_, [this]() mutable -> awaitable<void> {
        try {
            ATimePoint point = std::chrono::steady_clock::now();
            point = std::chrono::floor<std::chrono::seconds>(point);

            while (!ctx_.stopped()) {
                point += std::chrono::seconds(1);
                timer_.expires_at(point);
                co_await timer_.async_wait();

                for (const auto mgr: std::views::values(mgrMap_))
                    mgr->OnTick(point);
            }
        } catch (std::exception &e) {
            spdlog::warn("{}", e.what());
        }
    }, detached);
}

AThreadID UManagerSystem::GetThreadID() const {
    return tid_;
}

bool UManagerSystem::InManagerThread() const {
    return tid_ == std::this_thread::get_id();
}
