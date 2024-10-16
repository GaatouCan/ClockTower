#pragma once

#include <asio.hpp>
#include "../base/IPackage.h"

namespace base {

    struct LoginInfo {
        uint64_t pid;
        std::string token;
    };

    class ILoginHandler {
    public:
        virtual ~ILoginHandler() = default;

        virtual LoginInfo ParseLoginInfo(IPackage *) = 0;
        virtual asio::awaitable<void> OnPlayerLogin(const std::shared_ptr<class Connection> &conn, const LoginInfo &) = 0;
    };

    template<PACKAGE_TYPE T>
    class TLoginHandler : public ILoginHandler {
    public:
        LoginInfo ParseLoginInfo(IPackage *pkg) override {
            return ParseLoginInfoT(dynamic_cast<T *>(pkg));
        }

        virtual LoginInfo ParseLoginInfoT(T *) = 0;
    };
}
