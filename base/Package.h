#pragma once

#include <concepts>

/**
 * 抽象数据包基类
 */
class IPackage {

    class UPackagePool *mOwnerPool = nullptr;

public:
    virtual ~IPackage() = default;

    void SetOwnerPool(UPackagePool *pool) {
        mOwnerPool = pool;
    }

    [[nodiscard]] UPackagePool *GetOwnerPool() const {
        return mOwnerPool;
    }

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

    virtual void CopyFromOther(IPackage *other) = 0;
};

template<typename T>
concept PACKAGE_TYPE = std::derived_from<T, IPackage>;
