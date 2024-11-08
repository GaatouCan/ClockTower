#pragma once

#include <concepts>

/**
 * 抽象数据包基类
 */
class IPackage {
public:
    virtual ~IPackage() = default;

    /**
     * 获取数据包ID
     * @return uint32_t
     */
    [[nodiscard]] virtual uint32_t GetID() const { return 0; }

    /**
     * 清空数据包内所有数据
     */
    virtual void Reset() = 0;

    /**
     * 使数据包变为失效状态
     */
    virtual void Invalid() = 0;

    /**
     * 判断数据包是否可用
     * @return bool
     */
    [[nodiscard]] virtual bool IsAvailable() const = 0;
};

template<typename T>
concept PACKAGE_TYPE = std::derived_from<T, IPackage>;
