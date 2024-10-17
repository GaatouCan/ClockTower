#pragma once

#include <unordered_map>
#include <typeindex>

#include "IPlayerComponent.h"

class ComponentModule final {

    class Player *owner_;

public:
    ComponentModule() = delete;

    explicit ComponentModule(Player *plr);
    ~ComponentModule();

    [[nodiscard]] Player *GetOwner() const;

    template<typename T>
    requires std::derived_from<T, IPlayerComponent>
    T *CreateComponent() {
        auto res = new T(this);
        componentMap_[typeid(T)] = res;
        return res;
    }

    template<typename T>
    requires std::derived_from<T, IPlayerComponent>
    T *GetComponent() {
        if (const auto it = componentMap_.find(typeid(T)); it != componentMap_.end()) {
            return dynamic_cast<T *>(it->second);
        }
        return nullptr;
    }

private:
    std::unordered_map<std::type_index, IPlayerComponent *> componentMap_;
};
