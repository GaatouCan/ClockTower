#pragma once

#include <typeindex>
#include <mysqlx/xdevapi.h>

#include "PlayerComponent.h"

class UComponentModule final {

    class UPlayer *owner_;

    using ASerializeHandler = std::function<void(IPlayerComponent *, mysqlx::Table)>;
    using ADeserializeHandler = std::function<void(IPlayerComponent *, mysqlx::RowResult)>;

    struct FSerializeNode {
        ASerializeHandler serializer;
        ADeserializeHandler deserializer;
    };

    struct FComponentNode {
        IPlayerComponent * comp = nullptr;
        std::map<std::string, FSerializeNode> serializers;
    };

    std::map<std::type_index, FComponentNode> componentMap_;

public:
    UComponentModule() = delete;

    explicit UComponentModule(UPlayer *plr);
    ~UComponentModule();

    [[nodiscard]] UPlayer *GetOwner() const;

    template<typename T>
    requires std::derived_from<T, IPlayerComponent>
    T *CreateComponent() {
        componentMap_.insert_or_assign(typeid(T),  FComponentNode{});
        auto comp = new T(this);
        componentMap_[typeid(T)].comp = comp;
        return comp;
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
    void RegisterSerializer(const std::string &table, const ASerializeHandler &handler) {
        if (!componentMap_[typeid(T)].serializers.contains(table)) {
            componentMap_[typeid(T)].serializers[table] = FSerializeNode{};
        }
        componentMap_[typeid(T)].serializers[table].serializer = handler;
    }

    template<typename T>
    requires std::derived_from<T, IPlayerComponent>
    void RegisterDeserializer(const std::string &table, const ADeserializeHandler &handler) {
        if (!componentMap_[typeid(T)].serializers.contains(table)) {
            componentMap_[typeid(T)].serializers[table] = FSerializeNode{};
        }
        componentMap_[typeid(T)].serializers[table].deserializer = handler;
    }

    void Serialize(mysqlx::Schema &schema);
    void Deserialize(mysqlx::Schema &schema);

    void OnLogin();
    void OnLogout();

    void SyncCache(FCacheNode *node);
};

/**
 * 注册组件序列化和反序列化调用
 * @param comp 组件类型
 * @param tb 数据库表名
 */
#define SERIALIZE_COMPONENT(comp, tb) \
{ \
    GetModule()->RegisterSerializer<comp>(PascalToUnderline(#tb), [](IPlayerComponent *ct, mysqlx::Table table) { \
        try { \
            USerializer serializer(std::move(table)); \
            dynamic_cast<comp *>(ct)->Serialize_##tb(serializer); \
        } catch (std::bad_cast &e) { \
            spdlog::error("{} - {} Serializer: {}", #comp, #tb, e.what()); \
        } \
    }); \
    GetModule()->RegisterDeserializer<comp>(PascalToUnderline(#tb), [](IPlayerComponent *ct, mysqlx::RowResult res) { \
        try { \
            UDeserializer deserializer(std::move(res)); \
            dynamic_cast<comp *>(ct)->Deserialize_##tb(deserializer); \
        } catch (std::bad_cast &e) { \
            spdlog::error("{} - {} deserializer: {}", #comp, #tb, e.what()); \
        } \
    }); \
}