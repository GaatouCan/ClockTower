#include "CrossWorld.h"

#include <spdlog/spdlog.h>

UCrossWorld::UCrossWorld()
    : mAcceptor(mContext), bRunning(false) {
}

UCrossWorld::~UCrossWorld() {
    Shutdown();
}

UCrossWorld &UCrossWorld::Init() {
    return *this;
}

UCrossWorld & UCrossWorld::Run() {
    asio::signal_set signals(mContext, SIGINT, SIGTERM);
    signals.async_wait([&](auto, auto) {
        Shutdown();
    });

    co_spawn(mContext, WaitForWorld(), detached);
    mContext.run();

    bRunning = true;
    return *this;
}

UCrossWorld &UCrossWorld::Shutdown() {
    if (!mContext.stopped())
        mContext.stop();

    bRunning = false;
    return *this;
}

awaitable<void> UCrossWorld::WaitForWorld() {
    try {
        mAcceptor.open(tcp::v4());
        mAcceptor.bind({tcp::v4(), 8050});
        mAcceptor.listen();

        while (bRunning) {
            if (auto socket = co_await mAcceptor.async_accept(mContext); socket.is_open()) {
                // TODO
            }
        }
    } catch (std::exception &e) {
        spdlog::error("{} - {}", __FUNCTION__, e.what());
        Shutdown();
    }
}
