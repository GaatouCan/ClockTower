#pragma once

#include "../../sub_system.h"
#include "../../LoginHandler.h"

class UConnection;
using AConnectionPointer = std::shared_ptr<UConnection>;

class ULoginSystem final : public ISubSystem {

    std::unique_ptr<ILoginHandler> mHandler;

public:
    void Init() override;

    [[nodiscard]] constexpr const char * GetSystemName() const override {
        return "ULoginSystem";
    }

    bool VerifyAddress(const asio::ip::address &addr);

    FPlayerID VerifyToken(FPlayerID pid, const std::string &token);

    awaitable<void> OnLogin(const AConnectionPointer &conn, class IPackage *pkg);

    template<typename T>
    requires std::derived_from<T, ILoginHandler>
    void SetHandler() {
        if (mHandler != nullptr) {
            mHandler.reset();
        }
        mHandler = std::make_unique<T>();
    }
};
