#pragma once

#include "../../sub_system.h"
#include "../../protocol_handler.h"

#include <concepts>

class UConnection;

class UProtocolSystem final : public ISubSystem {

    std::unordered_map<uint32_t, AProtoFunctor> mProtoMap;
    std::unique_ptr<IProtocolHandler> mHandler;

public:
    void Init() override;

    [[nodiscard]] constexpr const char *GetSystemName() const override { return "UProtocolSystem"; }

    void RegisterProtocol(uint32_t type, const AProtoFunctor &func);

    [[nodiscard]] AProtoFunctor Find(uint32_t proto) const;

    template<typename T>
    requires std::derived_from<T, IProtocolHandler>
    void SetHandler() {
        if (mHandler != nullptr)
            mHandler.reset();

        mHandler = std::make_unique<T>();
    }

    awaitable<void> OnReadPackage(const std::shared_ptr<UConnection> &conn, IPackage *pkg) const;
};

#define REGISTER_PROTOCOL(proto) \
    sys->RegisterProtocol(static_cast<uint32_t>(EProtoType::proto), &proto);
