#include "AppearanceCT.h"

#include "../base/impl/Package.h"
#include "../base/utils.h"
#include "../../../player/ComponentModule.h"
#include "../../../player/Player.h"
#include "../../../common/proto_def.h"

#include <ranges>
#include <utility>
#include <spdlog/spdlog.h>
#include <appearance.pb.h>

UAppearanceCT::UAppearanceCT(UComponentModule *module)
    : IPlayerComponent(module) {

    SERIALIZE_COMPONENT(UAppearanceCT, Appearance)
    SERIALIZE_COMPONENT(UAppearanceCT, Avatar)
    SERIALIZE_COMPONENT(UAppearanceCT, AvatarFrame)
}

void UAppearanceCT::Serialize_Appearance(USerializer &s) {
    s.Serialize(&appearance_);
}

void UAppearanceCT::Deserialize_Appearance(UDeserializer &ds) {
    if (ds.HasMore()) {
        ds.Deserialize(&appearance_);
    }
}

void UAppearanceCT::Serialize_Avatar(USerializer &s) {
    for (auto &avatar: std::views::values(avatarMap_)) {
        s.Serialize(&avatar);
    }
}

void UAppearanceCT::Deserialize_Avatar(UDeserializer &ds) {
    while (ds.HasMore()) {
        orm::UDBTable_Avatar avatar;
        ds.Deserialize(&avatar);
        avatarMap_[avatar.index] = avatar;
    }
}

void UAppearanceCT::Serialize_AvatarFrame(USerializer &s) {
    for (auto &frame: std::views::values(avatarFrameMap_)) {
        s.Serialize(&frame);
    }
}

void UAppearanceCT::Deserialize_AvatarFrame(UDeserializer &ds) {
    while (ds.HasMore()) {
        orm::UDBTable_AvatarFrame frame;
        ds.Deserialize(&frame);
        avatarFrameMap_[frame.index] = frame;
    }
}

void UAppearanceCT::SendInfo() const {
    Appearance::SC_AppearanceResponse res;

    res.set_current_avatar(appearance_.avatar);
    res.set_current_avatar_frame(appearance_.avatar_frame);

    for (auto &val: std::views::values(avatarMap_)) {
        const auto avatar = res.add_avatar();
        avatar->set_index(val.index);
        avatar->set_bactivated(val.activated);
        avatar->set_expired(val.expired_time);
    }

    for (auto &val: std::views::values(avatarFrameMap_)) {
        const auto frame = res.add_avatar_frame();
        frame->set_index(val.index);
        frame->set_bactivated(val.activated);
        frame->set_expired(val.expired_time);
    }

    // GetOwner()->SendPackage(SC_AppearanceResponse, res);
}

awaitable<void> protocol::CS_AppearanceRequest(const std::shared_ptr<UCharacter> &character, IPackage *pkg) {
    if (character == nullptr)
        co_return;

    const auto player = std::dynamic_pointer_cast<UPlayer>(character);
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