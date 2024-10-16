#pragma once

#include "../common/common.h"

#include <queue>
#include <yaml-cpp/yaml.h>

namespace base {

    class IPackage;

    class PackagePool final {

        std::queue<IPackage *> queue_;
        size_t useCount_ = 0;

        static size_t defaultCapacity;
        static size_t minCapacity;

        static float expanseRate;
        static float expanseScale;

        static float collectRate;
        static float collectScale;

    public:
        explicit PackagePool(size_t capacity = defaultCapacity);
        ~PackagePool();

        DISABLE_COPY_MOVE(PackagePool)

        IPackage *acquire();
        void recycle(IPackage *pkg);

        static void readConfig(const YAML::Node &cfg);

        static void setDefaultCapacity(size_t capacity);
        static void setMinimumCapacity(size_t capacity);

        static void setExpanseRate(float rate);
        static void setExpanseScale(float scale);

        static void setCollectRate(float rate);
        static void setCollectScale(float scale);

    private:
        void expanse();
        void collect();
    };
} // base
