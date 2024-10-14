#include "PackagePool.h"
#include "../common/utils.h"

namespace base {

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

    void PackagePool::expanse() {
        if (std::floor(queue_.size() / useCount_) >= PACKAGE_POOL_EXPANSE_RATE)
            return;

        const auto num = static_cast<size_t>(std::ceil(static_cast<float>(useCount_) * PACKAGE_POOL_EXPANSE_SCALE));
        for (size_t i = 0; i < num; i++) {
            queue_.emplace(CreatePackage());
        }
    }

    void PackagePool::collect() {
        if (queue_.size() <= PACKAGE_POOL_MINIMUM_CAPACITY || std::floor(queue_.size() / useCount_) < PACKAGE_POOL_COLLECT_RATE)
            return;

        const auto num = static_cast<size_t>(std::ceil(static_cast<float>(queue_.size()) * PACKAGE_POOL_COLLECT_SCALE));
        for (size_t i = 0; i < num && queue_.size() > PACKAGE_POOL_MINIMUM_CAPACITY; i++) {
            const auto pkg = queue_.front();
            queue_.pop();
            delete pkg;
        }
    }
} // base