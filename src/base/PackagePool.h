#pragma once

#include "../common/common.h"

#include <queue>

namespace base {

    class IPackage;

    static constexpr size_t PACKAGE_POOL_DEFAULT_CAPACITY = 64;
    static constexpr size_t PACKAGE_POOL_MINIMUM_CAPACITY = 16;

    static constexpr float PACKAGE_POOL_EXPANSE_RATE = 0.3f;
    static constexpr float PACKAGE_POOL_EXPANSE_SCALE = 0.7f;

    static constexpr float PACKAGE_POOL_COLLECT_RATE = 1.f;
    static constexpr float PACKAGE_POOL_COLLECT_SCALE = 0.7f;

    class PackagePool final {

        std::queue<IPackage *> queue_;
        size_t useCount_ = 0;

    public:
        explicit PackagePool(size_t capacity = PACKAGE_POOL_DEFAULT_CAPACITY);
        ~PackagePool();

        DISABLE_COPY_MOVE(PackagePool)

        IPackage *acquire();
        void recycle(IPackage *pkg);

    private:
        void expanse();
        void collect();
    };
} // base
