#pragma once

#include "../../base/ISubSystem.h"
#include "IManager.h"
#include "../../base/GameWorld.h"

#include <typeindex>

namespace base {

    class ManagerSystem final : public ISubSystem {
        SUB_SYSTEM_BODY(ManagerSystem)

        void init() override;

        template<MANAGER_TYPE T>
        void createManager(asio::io_context &ctx) {
            mgrMap_[typeid(T)] = new T(ctx);
        }

    public:
        template<MANAGER_TYPE T>
        T *getManager() {
            if (const auto it = mgrMap_.find(typeid(T)); it != mgrMap_.end()) {
                return dynamic_cast<T *>(it->second);
            }
            return nullptr;
        }

    private:
        std::unordered_map<std::type_index, IManager *> mgrMap_;
    };
} // base

template<base::MANAGER_TYPE T>
T *GetManager() {
    const auto sys = GetSystem<base::ManagerSystem>();
    if (sys == nullptr)
        throw std::runtime_error("Cannot find ManagerSystem");

    return sys->getManager<T>();
}