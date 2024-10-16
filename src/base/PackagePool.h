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

        static void LoadConfig(const YAML::Node &cfg);

        static void SetDefaultCapacity(size_t capacity);
        static void SetMinimumCapacity(size_t capacity);

        static void SetExpanseRate(float rate);
        static void SetExpanseScale(float scale);

        static void SetCollectRate(float rate);
        static void SetCollectScale(float scale);

    private:
        void expanse();
        void collect();
    };
} // base
