#pragma once

#include "../LoginHandler.h"
#include "../../Package.h"
#include "../../SubSystem.h"


class UConnection;

class ULoginSystem final : public ISubSystem {

    SUB_SYSTEM_BODY(LoginSystem)
    void Init() override;

public:
    bool VerifyAddress(const asio::ip::address &addr);

    uint64_t VerifyToken(uint64_t pid, const std::string &token);

    awaitable<void> OnLogin(const std::shared_ptr<UConnection> &conn, IPackage *pkg);

    template<typename T>
    requires std::derived_from<T, ILoginHandler>
    void SetHandler() {
        if (mHandler != nullptr) {
            mHandler.reset();
        }
        mHandler = std::make_unique<T>();
    }

private:
    std::unique_ptr<ILoginHandler> mHandler;
};