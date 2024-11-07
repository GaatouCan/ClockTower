#pragma once

#include "common.h"
#include "MultiContextPool.h"
#include "Connection.h"
#include "SubSystem.h"

#include <typeindex>


UGameWorld &GetWorld();

class UGameWorld final {

    asio::io_context mContext;
    ATcpAcceptor mAcceptor;

    UMultiContextPool mPool;

    std::map<std::string, AConnectionPointer> mConnectionMap;
    ASteadyTimer mFullTimer;

    struct FSystemPriority {
        int priority;
        std::type_index type;

        bool operator<(const FSystemPriority &other) const {
            return priority < other.priority;
        }

        bool operator>(const FSystemPriority &other) const {
            return priority > other.priority;
        }
    };

    std::priority_queue<FSystemPriority, std::vector<FSystemPriority>, std::greater<> > mInitPriority;
    std::priority_queue<FSystemPriority, std::vector<FSystemPriority>, std::less<> > mDestPriority;
    std::unordered_map<std::type_index, ISubSystem *> mSystemMap;

    std::function<void(const AConnectionPointer &)> mConnectionFilter;

    AThreadID mMainThreadId;

    bool bInited;
    std::atomic_bool bRunning;

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
        if (const auto iter = mSystemMap.find(typeid(T)); iter != mSystemMap.end())
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
        if (bInited)
            return nullptr;

        const auto res = new T();

        mSystemMap.insert_or_assign(typeid(T), res);
        mInitPriority.push({priority, typeid(T)});
        mDestPriority.push({priority, typeid(T)});

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
