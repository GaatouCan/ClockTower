#pragma once

#include "common.h"

#include <queue>
#include <set>
#include <shared_mutex>
#include <yaml-cpp/yaml.h>


class IPackage;

/**
 * 数据包池
 */
class UPackagePool final {

    std::queue<IPackage *> mQueue;
    std::set<IPackage *> mInUseSet;
    std::atomic<ATimePoint> mCollectTime;

    std::mutex mMutex;
    mutable std::shared_mutex mSharedMutex;

    // 扩容和收缩临界点和比例
    // 每个线程下的数据包池行为目前设计为一致

    static size_t sDefaultCapacity;
    static size_t sMinCapacity;

    static float sExpanseRate;
    static float sExpanseScale;

    static float sCollectRate;
    static float sCollectScale;

    static std::function<IPackage*()> sCreatePackage;
    static std::function<void(IPackage*)> sInitPackage;

public:
    explicit UPackagePool(size_t capacity = sDefaultCapacity);
    ~UPackagePool();

    DISABLE_COPY_MOVE(UPackagePool)

    [[nodiscard]] size_t GetCapacity() const;

    /**
     * 从池里获取一个数据包
     * @return 数据包基类指针
     */
    IPackage *Acquire();

    /**
     * 回收数据包
     * @param pkg 被回收数据包指针
     */
    void Recycle(IPackage *pkg);

    /**
     * 加载全局配置 定义扩容和收缩临界点和比例
     * @param cfg 全局配置
     */
    static void LoadConfig(const YAML::Node &cfg);

    static void SetDefaultCapacity(size_t capacity);
    static void SetMinimumCapacity(size_t capacity);

    static void SetExpanseRate(float rate);
    static void SetExpanseScale(float scale);

    static void SetCollectRate(float rate);
    static void SetCollectScale(float scale);

    static void SetPackageBuilder(const std::function<IPackage*()>& func);
    static void SetPackageInitializer(const std::function<void(IPackage*)>& func);

private:
    void Expanse();
    void Reduce();
};
