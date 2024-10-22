#include "AppearanceCT.h"

#include "../player/ComponentModule.h"
#include "../common/utils.h"

#include <ranges>
#include <utility>
#include <spdlog/spdlog.h>

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
