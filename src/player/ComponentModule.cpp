#include "ComponentModule.h"
#include "Player.h"

ComponentModule::ComponentModule(Player *plr)
    : owner_(plr){
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
                node.serializer(comp, table);
            }
        }
    }
}

void ComponentModule::Deserialize(mysqlx::Schema &schema) {
    for (const auto& [comp, serialize] : std::views::values(componentMap_)) {
        for (const auto& [tableName, node] : serialize) {
            if (mysqlx::Table table = schema.getTable(tableName); table.existsInDatabase()) {
                mysqlx::RowResult result = table.select().where("pid = :pid").bind("pid", GetOwner()->GetPlayerID()).execute();
                node.deserializer(comp, std::move(result));
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
