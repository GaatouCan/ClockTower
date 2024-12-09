#include "AppearanceCT.h"

#include "../../../player/ComponentModule.h"
#include "../../../player/Player.h"
#include "../../../common/proto_def.h"

#include <impl/package.h>
#include <utils.h>

#include <ranges>
#include <utility>
#include <spdlog/spdlog.h>
#include <appearance.pb.h>

#include "game_world.h"
#include "system/config/ConfigSystem.h"

UAppearanceCT::UAppearanceCT(UComponentModule *module)
    : IPlayerComponent(module) {

    SERIALIZE_COMPONENT(UAppearanceCT, Appearance)
    SERIALIZE_COMPONENT(UAppearanceCT, Avatar)
    SERIALIZE_COMPONENT(UAppearanceCT, AvatarFrame)
}

void UAppearanceCT::Serialize_Appearance(USerializer &s) {
    s.Serialize(&mAppearance);
}

void UAppearanceCT::Deserialize_Appearance(UDeserializer &ds) {
    if (ds.HasMore()) {
        ds.Deserialize(&mAppearance);
    }
}

void UAppearanceCT::Serialize_Avatar(USerializer &s) {
    for (auto &avatar: std::views::values(mAvatarMap)) {
        s.Serialize(&avatar);
    }
}

void UAppearanceCT::Deserialize_Avatar(UDeserializer &ds) {
    while (ds.HasMore()) {
        orm::UDBTable_Avatar avatar;
        ds.Deserialize(&avatar);
        mAvatarMap[avatar.index] = avatar;
    }
}

void UAppearanceCT::Serialize_AvatarFrame(USerializer &s) {
    for (auto &frame: std::views::values(mAvatarFrameMap)) {
        s.Serialize(&frame);
    }
}

void UAppearanceCT::Deserialize_AvatarFrame(UDeserializer &ds) {
    while (ds.HasMore()) {
        orm::UDBTable_AvatarFrame frame;
        ds.Deserialize(&frame);
        mAvatarFrameMap[frame.index] = frame;
    }
}

void UAppearanceCT::SendInfo() const {
    Appearance::SC_AppearanceResponse res;

    res.set_current_avatar(mAppearance.avatar);
    res.set_current_avatar_frame(mAppearance.avatar_frame);

    for (auto &val: std::views::values(mAvatarMap)) {
        const auto avatar = res.add_avatar();
        avatar->set_index(val.index);
        avatar->set_bactivated(val.activated);
        avatar->set_expired(val.expired_time);
    }

    for (auto &val: std::views::values(mAvatarFrameMap)) {
        const auto frame = res.add_avatar_frame();
        frame->set_index(val.index);
        frame->set_bactivated(val.activated);
        frame->set_expired(val.expired_time);
    }

    SEND_PACKAGE(this, SC_AppearanceResponse, res)
}

void UAppearanceCT::ActiveAvatar(const int index) {
    if (const auto it = mAvatarMap.find(index); it != mAvatarMap.end()) {
        if (it->second.activated) {
            SendInfo();
            return;
        }
    }

    const auto cfg = GetSystem<UConfigSystem>()->FindConfig("appearance.avatar", index);
    if (!cfg.has_value())
        return;

    if (!mAvatarMap.contains(index)) {
        mAvatarMap[index] = orm::UDBTable_Avatar();
    }

    mAvatarMap[index].activated = true;
    if (const auto expired = (*cfg)["expired"].get<uint64_t>(); expired > 0) {
        mAvatarMap[index].expired_time = UnixTime() + expired;
    }

    mAppearance.update_time = UnixTime();
    SendInfo();
}

awaitable<void> protocol::CS_AppearanceRequest(const std::shared_ptr<IAbstractPlayer> &plr, IPackage *pkg) {
    if (plr == nullptr)
        co_return;

    const auto player = std::dynamic_pointer_cast<UPlayer>(plr);
    const auto package = dynamic_cast<FPackage *>(pkg);

    Appearance::CS_AppearanceRequest req;
    req.ParseFromString(package->GetData());

    const auto ct = player->GetComponentModule().GetComponent<UAppearanceCT>();
    if (ct == nullptr)
        co_return;

    switch (req.operate_type()) {
        case Appearance::SEND_INFO: {
            ct->SendInfo();
        } break;
        default: break;
    }
}
