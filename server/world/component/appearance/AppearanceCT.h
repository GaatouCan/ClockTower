#pragma once

#include "../../../player/PlayerComponent.h"

#include <system/database/Serializer.h>
#include <system/database/Deserializer.h>
#include <orm/appearance.orm.h>

class UAppearanceCT final : public IPlayerComponent {

    orm::UDBTable_Appearance mAppearance;
    std::map<uint32_t, orm::UDBTable_Avatar> mAvatarMap;
    std::map<uint32_t, orm::UDBTable_AvatarFrame> mAvatarFrameMap;

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

