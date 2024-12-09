#include "protocol_system.h"
#include "../../package.h"

#include <spdlog/spdlog.h>

void UProtocolSystem::Init() {

}

void UProtocolSystem::RegisterProtocol(const uint32_t type, const AProtoFunctor &func) {
    mProtoMap[type] = func;
}

AProtoFunctor UProtocolSystem::Find(const uint32_t proto) const {
    if (const auto it = mProtoMap.find(proto); it != mProtoMap.end()) {
        return it->second;
    }
    return AProtoFunctor();
}

awaitable<void> UProtocolSystem::OnReadPackage(const std::shared_ptr<UConnection> &conn, IPackage *pkg) const {
    if (mHandler == nullptr) {
        spdlog::critical("{} - handler not set.", __FUNCTION__);
        co_return;
    }

    if (!pkg->IsAvailable()) {
        spdlog::warn("{} - Package unavailable", __FUNCTION__);
        co_return;
    }

    if (const auto func = Find(pkg->GetID()); func) {
       co_await mHandler->Execute(func, conn, pkg);
    } else {
        spdlog::warn("{} - Package[{}] Protocol functor unavailable.", __FUNCTION__, pkg->GetID());
    }
}
