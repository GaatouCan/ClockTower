#include "Player.h"

#include "../common/ProtoType.h"

#include <game_world.h>
#include <impl/package.h>
#include <system/event/EventSystem.h>
#include <system/database/DatabaseSystem.h>

#include <utility>
#include <ranges>
#include <login.pb.h>


UPlayer::UPlayer(AConnectionPointer conn)
    : IAbstractPlayer(std::move(conn)),
      mComponentModule(this),
      mEventModule(this) {
}

UPlayer::~UPlayer() {
    spdlog::trace("{} - {}", __FUNCTION__, GetFullID());
}

UComponentModule &UPlayer::GetComponentModule() {
    return mComponentModule;
}

UEventModule &UPlayer::GetEventModule() {
    return mEventModule;
}

void UPlayer::OnDayChange() {
    mComponentModule.OnDayChange();
}

awaitable<void> UPlayer::OnLogin() {
    if (!IsSameThread()) {
        RunInThread(&UPlayer::OnLogin, this);
        co_return;
    }
    mLoginTime = NowTimePoint();
    spdlog::info("{} - Player[{}] Login Successfully.", __FUNCTION__, GetFullID());

    if (const auto sys = GetSystem<UDatabaseSystem>(); sys != nullptr) {
        const bool ret = co_await sys->AsyncTask([this](mysqlx::Schema &schema) {
            mComponentModule.Deserialize(schema);
            return true;
        }, asio::use_awaitable);
        if (!ret) {
            spdlog::warn("{} - Player[{}] Deserialize Failed.", __FUNCTION__, GetFullID());
        }
    }

    mComponentModule.OnLogin();

    Login::SC_LoginResponse response;
    response.set_result(true);
    response.set_progress(100);
    response.set_describe("Component Load Completed");

    SEND_PACKAGE(this, SC_LoginResponse, response)

    const auto param = new FEP_PlayerLogin;
    param->pid = GetFullID();

    DISPATCH_EVENT(PLAYER_LOGIN, param)
}

void UPlayer::OnLogout(const bool bForce, const std::string &otherAddress) {
    if (!IsSameThread()) {
        RunInThread(&UPlayer::OnLogin, this);
        return;
    }
    mLogoutTime = NowTimePoint();
    CleanAllTimer();

    if (const auto sys = GetSystem<UDatabaseSystem>(); sys != nullptr) {
        sys->PushTask([self = shared_from_this()](mysqlx::Schema &schema) {
            if (const auto plr = std::dynamic_pointer_cast<UPlayer>(self)) {
                plr->GetComponentModule().Serialize(schema);
                return true;
            }
            return false;
        }, [pid = this->GetFullID()](const bool ret) {
            if (!ret) {
                spdlog::info("UPlayer::OnLogout() - Player[{}] Serialize Success.", pid);
            } else {
                spdlog::warn("UPlayer::OnLogout() - Player[{}] Serialize Failed.", pid);
            }
        });
    }

    mComponentModule.OnLogout();
    spdlog::info("{} - Player[{}] Logout.", __FUNCTION__, GetFullID());

    if (bForce) {
        Login::SC_ForceLogoutResponse res;
        res.set_player_id(GetFullID());
        res.set_address(otherAddress);

        SEND_PACKAGE(this, SC_ForceLogoutResponse, res)
    }

    const auto param = new FEP_PlayerLogout;
    param->pid = GetFullID();

    DISPATCH_EVENT(PLAYER_LOGOUT, param);
}

bool UPlayer::IsOnline() const {
    constexpr ATimePoint zeroTimePoint{};
    const auto now = NowTimePoint();

    return (mLoginTime > zeroTimePoint && mLoginTime < now)
           && (mLogoutTime > zeroTimePoint && mLoginTime < now)
           && (mLogoutTime <= mLoginTime);
}


void UPlayer::Send(const uint32_t id, const std::string_view data) const {
    const auto pkg = dynamic_cast<FPackage *>(BuildPackage());
    pkg->SetPackageID(id).SetData(data);
    SendPackage(pkg);
}

void UPlayer::Send(const uint32_t id, const std::stringstream &ss) const {
    const auto pkg = dynamic_cast<FPackage *>(BuildPackage());
    pkg->SetPackageID(id).SetData(ss.str());
    SendPackage(pkg);
}

void UPlayer::SyncCache(FCacheNode *node) {
    mComponentModule.SyncCache(node);
}

std::shared_ptr<UPlayer> CreatePlayer(const AConnectionPointer &conn, const uint64_t pid) {
    auto plr = std::make_shared<UPlayer>(conn);
    return plr;
}
