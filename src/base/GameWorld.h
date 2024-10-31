#pragma once

#include "../common/common.h"
#include "MultiContextPool.h"
#include "Connection.h"
#include "SubSystem.h"

#include <typeindex>


UGameWorld &GetWorld();

class UGameWorld final {

    asio::io_context ctx_;
    ATcpAcceptor acceptor_;

    UMultiContextPool pool_;

    std::map<std::string, AConnectionPointer> connMap_;
    ASteadyTimer fullTimer_;

    struct SystemPriority {
        int priority;
        std::type_index type;

        bool operator<(const SystemPriority &other) const {
            return priority < other.priority;
        }

        bool operator>(const SystemPriority &other) const {
            return priority > other.priority;
        }
    };

    std::priority_queue<SystemPriority, std::vector<SystemPriority>, std::greater<> > initPriority_;
    std::priority_queue<SystemPriority, std::vector<SystemPriority>, std::less<> > destPriority_;
    std::unordered_map<std::type_index, ISubSystem *> systemMap_;

    std::function<void(const AConnectionPointer &)> connectionHandler_;

    AThreadID tid_;

    bool inited_;
    std::atomic_bool running_;

public:
    UGameWorld();
    ~UGameWorld();

    DISABLE_COPY_MOVE(UGameWorld)

    UGameWorld &Init();
    UGameWorld &Run();
    UGameWorld &Shutdown();

    void HandleConnection(const std::function<void(const AConnectionPointer &)> &handler);

    void RemoveConnection(const std::string &key);

    FContextNode &NextContextNode();

    asio::io_context &GetIOContext();

    AThreadID GetThreadID() const;

    template<SYSTEM_TYPE T>
    T *GetSystem() noexcept {
        if (const auto iter = systemMap_.find(typeid(T)); iter != systemMap_.end())
            return dynamic_cast<T *>(iter->second);
        return nullptr;
    }

    template<SYSTEM_TYPE T>
    class TSystemRegister {
    public:
        explicit TSystemRegister(const int priority = 0) {
            GetWorld().CreateSystem<T>(priority);
        }
    };

private:
    template<SYSTEM_TYPE T>
    T *CreateSystem(const int priority = 0) {
        if (inited_)
            return nullptr;

        const auto res = new T();

        systemMap_.insert_or_assign(typeid(T), res);
        initPriority_.push({priority, typeid(T)});
        destPriority_.push({priority, typeid(T)});

        return res;
    }

    awaitable<void> WaitForConnect();
};

/**
 * 在GameWorld初始化之前注册子系统
 * @param sys 子系统类型
 * @param priority 值越低约先初始化
 */
#define REGISTER_SYSTEM(sys, priority) \
    static UGameWorld::TSystemRegister<sys> g_##sys##_register(priority);

inline UGameWorld &GetWorld() {
    static UGameWorld world;
    return world;
}

template<SYSTEM_TYPE T>
T *GetSystem() {
    return GetWorld().GetSystem<T>();
}
