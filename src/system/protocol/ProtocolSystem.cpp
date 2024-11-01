#include "ProtocolSystem.h"
#include "../impl/ProtocolHandlerImpl.h"
#include "../../player/Player.h"
#include "../../common/proto_def.h"


UProtocolSystem::UProtocolSystem() {
    LoadProtocol(this);
}

void UProtocolSystem::Init() {
    if (handler_ == nullptr)
        SetHandler<ProtocolHandlerImpl>();
}

void UProtocolSystem::RegisterProtocol(const EProtoType type, const AProtoFunctor &func) {
    protoMap_[type] = func;
}

AProtoFunctor UProtocolSystem::Find(const EProtoType proto) const {
    if (const auto it = protoMap_.find(proto); it != protoMap_.end()) {
        return it->second;
    }
    return AProtoFunctor();
}

awaitable<void> UProtocolSystem::OnReadPackage(const std::shared_ptr<UConnection> &conn, IPackage *pkg) const {
    if (handler_ == nullptr) {
        spdlog::critical("{} - handler not set.", __func__);
        co_return;
    }

    if (!pkg->IsAvailable()) {
        spdlog::warn("{} - Package unavailable", __func__);
        co_return;
    }

    if (pkg->GetID() >= static_cast<uint32_t>(EProtoType::PROTO_TYPE_MAX)) {
        spdlog::warn("{} - Protocol type out of range", __func__);
        co_return;
    }


    if (const auto func = Find(static_cast<EProtoType>(pkg->GetID())); func) {
        co_await handler_->Execute(func, conn, pkg);
    } else {
        spdlog::warn("{} - Package[{}] Protocol functor unavailable.", __func__, pkg->GetID());
    }
}
