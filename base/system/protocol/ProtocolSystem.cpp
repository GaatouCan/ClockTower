#include "ProtocolSystem.h"
#include "../../Package.h"

// #include "../impl/ProtocolHandlerImpl.h"
// #include "../../player/Player.h"
// #include "../../common/proto_def.h"

#include <spdlog/spdlog.h>


void UProtocolSystem::Init() {

}

void UProtocolSystem::RegisterProtocol(const EProtoType type, const AProtoFunctor &func) {
    mProtoMap[type] = func;
}

AProtoFunctor UProtocolSystem::Find(const EProtoType proto) const {
    if (const auto it = mProtoMap.find(proto); it != mProtoMap.end()) {
        return it->second;
    }
    return AProtoFunctor();
}

awaitable<void> UProtocolSystem::OnReadPackage(const std::shared_ptr<UConnection> &conn, IPackage *pkg) const {
    if (mHandler == nullptr) {
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
        co_await mHandler->Execute(func, conn, pkg);
    } else {
        spdlog::warn("{} - Package[{}] Protocol functor unavailable.", __func__, pkg->GetID());
    }
}
