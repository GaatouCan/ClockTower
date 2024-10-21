#pragma once

#include <unordered_map>
#include <typeindex>
#include <mysqlx/xdevapi.h>

#include "IPlayerComponent.h"

class ComponentModule final {

    class Player *owner_;

    using SerializeHandler = std::function<void(IPlayerComponent *, mysqlx::Table &)>;
    using DeserializeHandler = std::function<void(IPlayerComponent *, mysqlx::RowResult)>;

    struct SerializeNode {
        SerializeHandler serializer;
        DeserializeHandler deserializer;
    };

    struct ComponentNode {
        IPlayerComponent * comp;
        std::unordered_map<std::string, SerializeNode> serializers;
    };

    std::unordered_map<std::type_index, ComponentNode> componentMap_;

public:
    ComponentModule() = delete;

    explicit ComponentModule(Player *plr);
    ~ComponentModule();

    [[nodiscard]] Player *GetOwner() const;

    template<typename T>
    requires std::derived_from<T, IPlayerComponent>
    T *CreateComponent() {
        componentMap_.insert_or_assign(typeid(T), {});
        auto res = new T(this);
        componentMap_[typeid(T)].comp = res;
        return res;
    }

    template<typename T>
    requires std::derived_from<T, IPlayerComponent>
    T *GetComponent() {
        if (const auto it = componentMap_.find(typeid(T)); it != componentMap_.end()) {
            return dynamic_cast<T *>(it->second.comp);
        }
        return nullptr;
    }

    template<typename T>
    requires std::derived_from<T, IPlayerComponent>
    void RegisterSerializer(const std::string &table, const SerializeHandler &handler) {
        if (!componentMap_[typeid(T)].serializers.contains(table)) {
            componentMap_[typeid(T)].serializers[table] = SerializeNode{};
        }
        componentMap_[typeid(T)].serializers[table].serializer = handler;
    }

    template<typename T>
    requires std::derived_from<T, IPlayerComponent>
    void RegisterDeserializer(const std::string &table, const DeserializeHandler &handler) {
        if (!componentMap_[typeid(T)].serializers.contains(table)) {
            componentMap_[typeid(T)].serializers[table] = SerializeNode{};
        }
        componentMap_[typeid(T)].serializers[table].deserializer = handler;
    }

    void Serialize(mysqlx::Schema &schema);
    void Deserialize(mysqlx::Schema &schema);

    void OnLogin();
    void OnLogout();
};
