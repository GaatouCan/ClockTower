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

        virtual LoginInfo parseLoginInfo(IPackage *) = 0;
        virtual asio::awaitable<void> login(const std::shared_ptr<class Connection> &conn, LoginInfo &) = 0;
    };

    template<PACKAGE_TYPE T>
    class TLoginHandler : public ILoginHandler {
    public:
        LoginInfo parseLoginInfo(IPackage *pkg) override {
            return parseLoginInfoT(dynamic_cast<T *>(pkg));
        }

        virtual LoginInfo parseLoginInfoT(T *) = 0;
    };
}
