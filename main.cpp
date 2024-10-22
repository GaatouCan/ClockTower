#include <spdlog/spdlog.h>
#include <asio.hpp>
#include <thread>


template<typename Callback>
void func(int a, Callback && cb) {
    std::thread([a, cb = std::forward<Callback>(cb)]() mutable  {
        spdlog::info("In func - {}", a);
        std::this_thread::sleep_for(std::chrono::seconds(1));

        std::stringstream ss;
        ss << std::this_thread::get_id();
        spdlog::info("func - {}", ss.str());

        std::move(cb)();
    }).detach();
}

template<asio::completion_token_for<void()> CompletionToken>
auto async_func(int a, CompletionToken &&token) {
    auto init = [](asio::completion_handler_for<void()> auto handler, int param) {
        auto work = asio::make_work_guard(handler);

        func(param, [handler = std::move(handler), work = std::move(work)]() mutable {
            auto alloc = asio::get_associated_allocator(handler, asio::recycling_allocator<void>());
            asio::dispatch(work.get_executor(), asio::bind_allocator(alloc, [handler = std::move(handler)]() mutable {
                std::move(handler)();
            }));
        });
    };

    return asio::async_initiate<CompletionToken, void()>(init, token, a);
}

asio::awaitable<void> test() {
    co_await async_func(11, asio::use_awaitable);
    spdlog::info("async completed");
    std::stringstream ss;
    ss << std::this_thread::get_id();
    spdlog::info("test - {}", ss.str());
}

int main() {
    spdlog::info("Hello World!");

    std::stringstream ss;
    ss << std::this_thread::get_id();
    spdlog::info("main - {}", ss.str());

    asio::io_context ctx;

    asio::co_spawn(ctx, test(), asio::detached);

    ctx.run();

    return 0;
}
