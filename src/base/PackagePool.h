#pragma once

#include "../common/common.h"

#include <queue>
#include <yaml-cpp/yaml.h>


class IPackage;

/**
 * 数据包池
 * @attention 单线程下使用 设计为一个数据包池对应一个线程下的io_context
 */
class UPackagePool final {
    std::queue<IPackage *> queue_;
    size_t useCount_ = 0;
    ATimePoint collectTime_;

    // 扩容和收缩临界点和比例
    // 每个线程下的数据包池行为目前设计为一致

    static size_t defaultCapacity;
    static size_t minCapacity;

    static float expanseRate;
    static float expanseScale;

    static float collectRate;
    static float collectScale;

public:
    explicit UPackagePool(size_t capacity = defaultCapacity);
    ~UPackagePool();

    DISABLE_COPY_MOVE(UPackagePool)

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

private:
    void Expanse();
    void Collect();
};
