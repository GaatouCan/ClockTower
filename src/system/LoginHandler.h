#pragma once

#include <asio.hpp>

struct FLoginInfo {
    uint64_t pid = 0;
    std::string token;
};

class ILoginHandler {
public:
    virtual ~ILoginHandler() = default;

    virtual FLoginInfo ParseLoginInfo(IPackage *) = 0;
    virtual awaitable<void> OnPlayerLogin(const std::shared_ptr<UConnection> &conn, const FLoginInfo &) = 0;
};
