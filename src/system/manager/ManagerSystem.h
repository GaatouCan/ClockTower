#pragma once

#include "../../base/ISubSystem.h"
#include "IManager.h"
#include "../../base/GameWorld.h"

#include <typeindex>

namespace base {

    class ManagerSystem final : public ISubSystem {
        SUB_SYSTEM_BODY(ManagerSystem)

        ManagerSystem();
        ~ManagerSystem() override;

        void Init() override;

        void LoadManager();

        template<MANAGER_TYPE T>
        void CreateManager(asio::io_context &ctx) {
            mgrMap_[typeid(T)] = new T(ctx);
        }

    public:
        template<MANAGER_TYPE T>
        T *GetManager() {
            if (const auto it = mgrMap_.find(typeid(T)); it != mgrMap_.end()) {
                return dynamic_cast<T *>(it->second);
            }
            return nullptr;
        }

    private:
        std::unordered_map<std::type_index, IManager *> mgrMap_;

        asio::io_context ctx_;
        SteadyTimer timer_;
        std::thread mgrThread_;
        ThreadID tid_;
    };
} // base

#define REGISTER_MANAGER(mgr) \
    CreateManager<mgr>(ctx_);

template<base::MANAGER_TYPE T>
T *GetManager() {
    const auto sys = GetSystem<base::ManagerSystem>();
    if (sys == nullptr)
        throw std::runtime_error("Cannot find ManagerSystem");

    return sys->GetManager<T>();
}