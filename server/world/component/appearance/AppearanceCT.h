#pragma once

#include "../../base/system/database/Serializer.h"
#include "../../base/system/database/Deserializer.h"
#include "../../../player/PlayerComponent.h"

#include <orm/appearance.orm.h>

class UAppearanceCT final : public IPlayerComponent {

    orm::UDBTable_Appearance appearance_;
    std::map<uint32_t, orm::UDBTable_Avatar> avatarMap_;
    std::map<uint32_t, orm::UDBTable_AvatarFrame> avatarFrameMap_;

public:
    explicit UAppearanceCT(UComponentModule *module);

    [[nodiscard]] constexpr const char * GetComponentName() const override {
        return "UAppearanceCT";
    }

    void Serialize_Appearance(USerializer &s);
    void Deserialize_Appearance(UDeserializer &ds);

    void Serialize_Avatar(USerializer &s);
    void Deserialize_Avatar(UDeserializer &ds);

    void Serialize_AvatarFrame(USerializer &s);
    void Deserialize_AvatarFrame(UDeserializer &ds);

    void SendInfo() const;
};

