#include "PlayerComponent.h"
#include "ComponentModule.h"
#include "Player.h"

IPlayerComponent::IPlayerComponent(UComponentModule *module)
    : mModule(module){
}

IPlayerComponent::~IPlayerComponent() {
}

UComponentModule * IPlayerComponent::GetModule() const {
    return mModule;
}

UPlayer * IPlayerComponent::GetOwner() const {
    if (mModule != nullptr)
        return mModule->GetOwner();
    return nullptr;
}

void IPlayerComponent::OnLogin() {
}

void IPlayerComponent::OnLogout() {
}

void IPlayerComponent::OnDayChange(bool bLogin) {
}

void IPlayerComponent::SyncCache(FCacheNode *node) {
}

void IPlayerComponent::Send(IPackage *pkg) const {
    if (const auto plr = GetOwner(); plr != nullptr) {
        plr->SendPackage(pkg);
    }
}

void IPlayerComponent::Send(const uint32_t id, const std::string_view data) const {
    if (const auto plr = GetOwner(); plr != nullptr) {
        plr->Send(id, data);
    }
}

void IPlayerComponent::Send(const uint32_t id, const std::stringstream &ss) const {
    if (const auto plr = GetOwner(); plr != nullptr) {
        plr->Send(id, ss);
    }
}
