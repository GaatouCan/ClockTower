#include "ProtocolSystem.h"
#include "../impl/ProtocolHandlerImpl.h"
#include "../../player/Player.h"


void UProtocolSystem::Init() {
    SetHandler<ProtocolHandlerImpl>();
    RegisterProtocol();
}

void UProtocolSystem::RegisterProtocol(const ProtoType type, const AProtoFunctor &func) {
    protoMap_[type] = func;
}

AProtoFunctor UProtocolSystem::Find(const ProtoType proto) const {
    if (const auto it = protoMap_.find(proto); it != protoMap_.end()) {
        return it->second;
    }
    return AProtoFunctor();
}

awaitable<void> UProtocolSystem::OnReadPackage(const std::shared_ptr<UConnection> &conn, IPackage *pkg) const {
    if (handler_ != nullptr) {
        if (const auto func = Find(static_cast<ProtoType>(pkg->GetID()))) {
            co_await handler_->Execute(func, conn, pkg);
        }
    }
}
