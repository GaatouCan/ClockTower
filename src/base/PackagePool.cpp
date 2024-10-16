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

    IPackage * PackagePool::acquire() {
        expanse();

        IPackage *pkg = queue_.front();
        queue_.pop();
        useCount_++;

        InitPackage(pkg);

        return pkg;
    }

    void PackagePool::recycle(IPackage *pkg) {
        if (pkg != nullptr) {
            pkg->reset();
            queue_.push(pkg);
            useCount_--;
        }

        collect();
    }

    void PackagePool::readConfig(const YAML::Node &cfg) {
        if (cfg["package"].IsNull() && cfg["package"]["pool"].IsNull())
            return;

        if (!cfg["package"]["pool"]["default_capacity"].IsNull())
            setDefaultCapacity(cfg["package"]["pool"]["default_capacity"].as<size_t>());

        if (!cfg["package"]["pool"]["minimum_capacity"].IsNull())
            setMinimumCapacity(cfg["package"]["pool"]["minimum_capacity"].as<size_t>());

        if (!cfg["package"]["pool"]["expanse_rate"].IsNull())
            setExpanseRate(cfg["package"]["pool"]["expanse_rate"].as<float>());

        if (!cfg["package"]["pool"]["expanse_scale"].IsNull())
            setExpanseScale(cfg["package"]["pool"]["expanse_scale"].as<float>());

        if (!cfg["package"]["pool"]["collect_rate"].IsNull())
            setCollectRate(cfg["package"]["pool"]["collect_rate"].as<float>());

        if (!cfg["package"]["pool"]["collect_scale"].IsNull())
            setCollectScale(cfg["package"]["pool"]["collect_scale"].as<float>());

        spdlog::info("Package pool config set.");
    }

    void PackagePool::setDefaultCapacity(const size_t capacity) {
        defaultCapacity = capacity;
    }

    void PackagePool::setMinimumCapacity(const size_t capacity) {
        minCapacity = capacity;
    }

    void PackagePool::setExpanseRate(const float rate) {
        expanseRate = rate;
    }

    void PackagePool::setExpanseScale(const float scale) {
        expanseScale = scale;
    }

    void PackagePool::setCollectRate(const float rate) {
        collectRate = rate;
    }

    void PackagePool::setCollectScale(const float scale) {
        collectScale = scale;
    }

    void PackagePool::expanse() {
        if (std::floor(queue_.size() / useCount_) >= expanseRate)
            return;

        const auto num = static_cast<size_t>(std::ceil(static_cast<float>(useCount_) * expanseScale));
        for (size_t i = 0; i < num; i++) {
            queue_.emplace(CreatePackage());
        }
    }

    void PackagePool::collect() {
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