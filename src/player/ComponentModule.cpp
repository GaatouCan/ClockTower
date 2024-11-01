#include "ComponentModule.h"
#include "Player.h"

// #include "../world/component/appearance/AppearanceCT.h"

UComponentModule::UComponentModule(UPlayer *plr)
    : owner_(plr){
    // CreateComponent<AppearanceCT>();
}

UComponentModule::~UComponentModule() {
    for (const auto& [comp, serialize] : std::views::values(componentMap_)) {
        delete comp;
    }
}

UPlayer * UComponentModule::GetOwner() const {
    return owner_;
}

void UComponentModule::Serialize(mysqlx::Schema &schema) {
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

void UComponentModule::Deserialize(mysqlx::Schema &schema) {
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

void UComponentModule::OnLogin() {
    for (const auto& [comp, serialize] : std::views::values(componentMap_)) {
        comp->OnLogin();
    }
}

void UComponentModule::OnLogout() {
    for (const auto& [comp, serialize] : std::views::values(componentMap_)) {
        comp->OnLogout();
    }
}

void UComponentModule::SyncCache(FCacheNode *node) {
    for (const auto& [comp, serialize] : std::views::values(componentMap_)) {
        comp->SyncCache(node);
    }
}
