#include "ComponentModule.h"
#include "Player.h"

#include "../world/component/appearance/AppearanceCT.h"

ComponentModule::ComponentModule(Player *plr)
    : owner_(plr){
    CreateComponent<AppearanceCT>();
}

ComponentModule::~ComponentModule() {
    for (const auto& [comp, serialize] : std::views::values(componentMap_)) {
        delete comp;
    }
}

Player * ComponentModule::GetOwner() const {
    return owner_;
}

void ComponentModule::Serialize(mysqlx::Schema &schema) {
    for (const auto& [comp, serialize] : std::views::values(componentMap_)) {
        for (const auto& [tableName, node] : serialize) {
            if (mysqlx::Table table = schema.getTable(tableName); table.existsInDatabase()) {
                if (node.serializer) {
                    node.serializer(comp, std::move(table));
                }
            }
        }
    }
}

void ComponentModule::Deserialize(mysqlx::Schema &schema) {
    for (const auto& [comp, serialize] : std::views::values(componentMap_)) {
        for (const auto& [tableName, node] : serialize) {
            if (mysqlx::Table table = schema.getTable(tableName); table.existsInDatabase()) {
                mysqlx::RowResult result = table.select().where("pid = :pid").bind("pid", GetOwner()->GetPlayerID()).execute();
                if (node.deserializer) {
                    node.deserializer(comp, std::move(result));
                }
            }
        }
    }
}

void ComponentModule::OnLogin() {
    for (const auto& [comp, serialize] : std::views::values(componentMap_)) {
        comp->OnLogin();
    }
}

void ComponentModule::OnLogout() {
    for (const auto& [comp, serialize] : std::views::values(componentMap_)) {
        comp->OnLogout();
    }
}

void ComponentModule::SyncCache(CacheNode *node) {
    for (const auto& [comp, serialize] : std::views::values(componentMap_)) {
        comp->SyncCache(node);
    }
}
