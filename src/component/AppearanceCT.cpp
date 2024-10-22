#include "AppearanceCT.h"
#include "../player/ComponentModule.h"
#include "../common/utils.h"

#include <utility>
#include <spdlog/spdlog.h>

AppearanceCT::AppearanceCT(ComponentModule *module)
    : IPlayerComponent(module) {

    SERIALIZE_COMPONENT(AppearanceCT, Appearance)
}

void AppearanceCT::Serialize_Appearance(base::Serializer &s) {
    s.Serialize(&appearance_);
}

void AppearanceCT::Deserialize_Appearance(base::Deserializer &ds) {
    if (ds.HasMore()) {
        ds.Deserialize(&appearance_);
    }
}
