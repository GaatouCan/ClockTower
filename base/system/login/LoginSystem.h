#pragma once

#include "../../Package.h"
#include "../../SubSystem.h"
#include "../../LoginHandler.h"

class UConnection;

class ULoginSystem final : public ISubSystem {

    std::unique_ptr<ILoginHandler> mHandler;

public:
    void Init() override;

    [[nodiscard]] constexpr const char * GetSystemName() const override {
        return "ULoginSystem";
    }

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
};
