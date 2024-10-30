#pragma once

#include <asio.hpp>

struct FLoginInfo {
    uint64_t pid = 0;
    std::string token;
};

class ILoginHandler {
public:
    virtual ~ILoginHandler() = default;

    virtual FLoginInfo ParseLoginInfo(class IPackage *) = 0;
    virtual asio::awaitable<void> OnPlayerLogin(const std::shared_ptr<class UConnection> &conn, const FLoginInfo &) = 0;
};
