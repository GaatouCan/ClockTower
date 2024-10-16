#include "PackagePool.h"
#include "../common/utils.h"

#include <spdlog/spdlog.h>

namespace base {

    size_t PackagePool::defaultCapacity = 64;
    size_t PackagePool::minCapacity = 16;

    float PackagePool::expanseRate = 0.3f;
    float PackagePool::expanseScale = 1.f;

    float PackagePool::collectRate = 1.f;
    float PackagePool::collectScale = 0.7f;

    PackagePool::PackagePool(const size_t capacity) {
        for (size_t i = 0; i < capacity; i++) {
            queue_.emplace(CreatePackage());
        }
    }

    PackagePool::~PackagePool() {
        while (!queue_.empty()) {
            const auto pkg = queue_.front();
            queue_.pop();
            delete pkg;
        }
    }

    IPackage * PackagePool::Acquire() {
        Expanse();

        IPackage *pkg = queue_.front();
        queue_.pop();
        useCount_++;

        InitPackage(pkg);

        return pkg;
    }

    void PackagePool::Recycle(IPackage *pkg) {
        if (pkg != nullptr) {
            pkg->Reset();
            queue_.push(pkg);
            useCount_--;
        }

        Collect();
    }

    void PackagePool::LoadConfig(const YAML::Node &cfg) {
        if (cfg["package"].IsNull() && cfg["package"]["pool"].IsNull())
            return;

        if (!cfg["package"]["pool"]["default_capacity"].IsNull())
            SetDefaultCapacity(cfg["package"]["pool"]["default_capacity"].as<size_t>());

        if (!cfg["package"]["pool"]["minimum_capacity"].IsNull())
            SetMinimumCapacity(cfg["package"]["pool"]["minimum_capacity"].as<size_t>());

        if (!cfg["package"]["pool"]["expanse_rate"].IsNull())
            SetExpanseRate(cfg["package"]["pool"]["expanse_rate"].as<float>());

        if (!cfg["package"]["pool"]["expanse_scale"].IsNull())
            SetExpanseScale(cfg["package"]["pool"]["expanse_scale"].as<float>());

        if (!cfg["package"]["pool"]["collect_rate"].IsNull())
            SetCollectRate(cfg["package"]["pool"]["collect_rate"].as<float>());

        if (!cfg["package"]["pool"]["collect_scale"].IsNull())
            SetCollectScale(cfg["package"]["pool"]["collect_scale"].as<float>());

        spdlog::info("Package pool config set.");
    }

    void PackagePool::SetDefaultCapacity(const size_t capacity) {
        defaultCapacity = capacity;
    }

    void PackagePool::SetMinimumCapacity(const size_t capacity) {
        minCapacity = capacity;
    }

    void PackagePool::SetExpanseRate(const float rate) {
        expanseRate = rate;
    }

    void PackagePool::SetExpanseScale(const float scale) {
        expanseScale = scale;
    }

    void PackagePool::SetCollectRate(const float rate) {
        collectRate = rate;
    }

    void PackagePool::SetCollectScale(const float scale) {
        collectScale = scale;
    }

    void PackagePool::Expanse() {
        if (std::floor(queue_.size() / useCount_) >= expanseRate)
            return;

        const auto num = static_cast<size_t>(std::ceil(static_cast<float>(useCount_) * expanseScale));
        for (size_t i = 0; i < num; i++) {
            queue_.emplace(CreatePackage());
        }
    }

    void PackagePool::Collect() {
        if (queue_.size() <= minCapacity || std::floor(queue_.size() / useCount_) < collectRate)
            return;

        const auto num = static_cast<size_t>(std::ceil(static_cast<float>(queue_.size()) * collectScale));
        for (size_t i = 0; i < num && queue_.size() > minCapacity; i++) {
            const auto pkg = queue_.front();
            queue_.pop();
            delete pkg;
        }
    }
} // base