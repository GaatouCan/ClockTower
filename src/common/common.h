#pragma once

#define DISABLE_COPY(clazz) \
    clazz(const clazz&) = delete; \
    clazz &operator=(const clazz&) = delete; \

#define DISABLE_MOVE(clazz) \
    clazz(clazz &&) noexcept = delete; \
    clazz &operator=(clazz &&) noexcept = delete;

#define DISABLE_COPY_MOVE(clazz) \
    DISABLE_COPY(clazz) \
    DISABLE_MOVE(clazz)

#include <chrono>

using ATimePoint = std::chrono::time_point<std::chrono::steady_clock>;

#include <asio.hpp>

using asio::ip::tcp;

using asio::co_spawn;
using asio::detached;
using asio::awaitable;

using asio::as_tuple_t;
using asio::use_awaitable_t;
using default_token = as_tuple_t<use_awaitable_t<> >;

using ATcpSocket = default_token::as_default_on_t<tcp::socket>;
using ASteadyTimer = default_token::as_default_on_t<asio::steady_timer>;
using ATcpAcceptor = default_token::as_default_on_t<tcp::acceptor>;

#include <thread>
using AThreadID = std::thread::id;