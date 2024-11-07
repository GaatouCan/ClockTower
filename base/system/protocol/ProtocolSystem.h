#pragma once

#include "../../SubSystem.h"
#include "../ProtocolHandler.h"

#include <concepts>

class UConnection;

class UProtocolSystem final : public ISubSystem {

    SUB_SYSTEM_BODY(ProtocolSystem)

    void Init() override;

public:
    void RegisterProtocol(EProtoType type, const AProtoFunctor &func);

    [[nodiscard]] AProtoFunctor Find(EProtoType proto) const;

    template<typename T>
    requires std::derived_from<T, IProtocolHandler>
    void SetHandler() {
        if (mHandler != nullptr)
            mHandler.reset();

        mHandler = std::make_unique<T>();
    }

    awaitable<void> OnReadPackage(const std::shared_ptr<UConnection> &conn, IPackage *pkg) const;

private:
    std::unordered_map<EProtoType, AProtoFunctor> mProtoMap;
    std::unique_ptr<IProtocolHandler> mHandler;
};

#define REGISTER_PROTOCOL(proto) \
    sys->RegisterProtocol(EProtoType::proto, &proto);
