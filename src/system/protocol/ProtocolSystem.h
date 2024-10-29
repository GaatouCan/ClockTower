#pragma once

#include "../../base/ISubSystem.h"
#include "../IProtocolHandler.h"

#include <concepts>


class UConnection;

class UProtocolSystem final : public ISubSystem {

    SUB_SYSTEM_BODY(ProtocolSystem)
    void Init() override;

public:
    void RegisterProtocol(ProtoType type, const AProtoFunctor &func);

    [[nodiscard]] AProtoFunctor Find(ProtoType proto) const;

    template<typename T>
    requires std::derived_from<T, IProtocolHandler>
    void SetHandler() {
        if (handler_ != nullptr)
            handler_.reset();

        handler_ = std::make_unique<T>();
    }

    awaitable<void> OnReadPackage(const std::shared_ptr<UConnection> &conn, IPackage *pkg) const;

private:
    std::unordered_map<ProtoType, AProtoFunctor> protoMap_;
    std::unique_ptr<IProtocolHandler> handler_;
};

#define REGISTER_PROTOCOL(proto) \
    handler_->Register(ProtoType::proto, &proto);
