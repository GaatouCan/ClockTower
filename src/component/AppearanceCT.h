#pragma once

#include "../system/database/Serializer.h"
#include "../system/database/Deserializer.h"
#include "../player/IPlayerComponent.h"
#include "../../struct/orm/appearance.orm.h"

class AppearanceCT final : public IPlayerComponent {

    orm::DBTable_Appearance appearance_;
    std::unordered_map<uint32_t, orm::DBTable_Avatar> avatarMap_;
    std::unordered_map<uint32_t, orm::DBTable_AvatarFrame> avatarFrameMap_;

public:
    explicit AppearanceCT(ComponentModule *module);

    void Serialize_Appearance(base::Serializer &s);
    void Deserialize_Appearance(base::Deserializer &ds);
};

