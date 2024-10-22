#include "AppearanceCT.h"

#include "../player/ComponentModule.h"
#include "../common/utils.h"
#include "../../protobuf/ProtoType.generated.h"
#include "../../protobuf/Protocol.generated.h"
#include "../player/Player.h"

#include <ranges>
#include <utility>
#include <spdlog/spdlog.h>

#include <appearance.pb.h>
#include <appearance.pb.cc>

AppearanceCT::AppearanceCT(ComponentModule *module)
    : IPlayerComponent(module) {

    SERIALIZE_COMPONENT(AppearanceCT, Appearance)
    SERIALIZE_COMPONENT(AppearanceCT, Avatar)
    SERIALIZE_COMPONENT(AppearanceCT, AvatarFrame)
}

void AppearanceCT::Serialize_Appearance(base::Serializer &s) {
    s.Serialize(&appearance_);
}

void AppearanceCT::Deserialize_Appearance(base::Deserializer &ds) {
    if (ds.HasMore()) {
        ds.Deserialize(&appearance_);
    }
}

void AppearanceCT::Serialize_Avatar(base::Serializer &s) {
    for (auto &avatar: std::views::values(avatarMap_)) {
        s.Serialize(&avatar);
    }
}

void AppearanceCT::Deserialize_Avatar(base::Deserializer &ds) {
    while (ds.HasMore()) {
        orm::DBTable_Avatar avatar;
        ds.Deserialize(&avatar);
        avatarMap_[avatar.index] = avatar;
    }
}

void AppearanceCT::Serialize_AvatarFrame(base::Serializer &s) {
    for (auto &frame: std::views::values(avatarFrameMap_)) {
        s.Serialize(&frame);
    }
}

void AppearanceCT::Deserialize_AvatarFrame(base::Deserializer &ds) {
    while (ds.HasMore()) {
        orm::DBTable_AvatarFrame frame;
        ds.Deserialize(&frame);
        avatarFrameMap_[frame.index] = frame;
    }
}

void AppearanceCT::SendInfo() const {
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

    GetOwner()->SendPackage(SC_AppearanceResponse, res);
}

void protocol::CS_AppearanceRequest(const std::shared_ptr<base::Player> &plr, base::Package *pkg) {

}