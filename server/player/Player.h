#pragma once

#include "ComponentModule.h"
#include "EventModule.h"

#include <system/scene/abstract_player.h>
#include <connection.h>
#include <repeated_timer.h>
#include <system/login/platform_info.h>
#include <system/event/event_param.h>

#include <spdlog/spdlog.h>

struct FEP_PlayerLogin final : IEventParam {
    uint64_t pid;
};

struct FEP_PlayerLogout final : IEventParam {
    uint64_t pid;
};

class UPlayer final : public IAbstractPlayer {

    AConnectionPointer mConn;

    ATimePoint mLoginTime;
    ATimePoint mLogoutTime;

    UComponentModule mComponentModule;
    UEventModule mEventModule;

    FPlatformInfo mPlatform;

public:
    UPlayer() = delete;

    explicit UPlayer(AConnectionPointer conn);
    ~UPlayer() override;

    UComponentModule &GetComponentModule();
    UEventModule &GetEventModule();

    void OnDayChange();

    awaitable<void> OnLogin();
    void OnLogout(bool bForce = false, const std::string &otherAddress = "");

    bool IsOnline() const;

    void Send(uint32_t id, std::string_view data) const;
    void Send(uint32_t id, const std::stringstream &ss) const;

    void SyncCache(struct FCacheNode *node);
};

#define SEND_PACKAGE(sender, proto, data) \
    (sender)->Send(static_cast<uint32_t>(protocol::EProtoType::proto), data.SerializeAsString());

#define BUILD_PACKAGE(pkg, proto, data) \
    pkg->SetPackageID(static_cast<uint32_t>(protocol::EProtoType::proto)).SetData(data.SerializeAsString());
