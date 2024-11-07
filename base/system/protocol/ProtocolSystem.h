#pragma once

#include "../../base/SubSystem.h"
#include "../ProtocolHandler.h"

#include <concepts>


class UConnection;

class UProtocolSystem final : public ISubSystem {

    SUB_SYSTEM_BODY(ProtocolSystem)

    UProtocolSystem();

    void Init() override;

public:
    void RegisterProtocol(EProtoType type, const AProtoFunctor &func);

    [[nodiscard]] AProtoFunctor Find(EProtoType proto) const;

    template<typename T>
    requires std::derived_from<T, IProtocolHandler>
    void SetHandler() {
        if (handler_ != nullptr)
            handler_.reset();

        handler_ = std::make_unique<T>();
    }

    awaitable<void> OnReadPackage(const std::shared_ptr<UConnection> &conn, IPackage *pkg) const;

private:
    std::unordered_map<EProtoType, AProtoFunctor> protoMap_;
    std::unique_ptr<IProtocolHandler> handler_;
};

#define REGISTER_PROTOCOL(proto) \
    sys->RegisterProtocol(EProtoType::proto, &proto);
