#include "PackagePool.h"
#include "../base/impl/Package.h"
#include "../system/config/ConfigSystem.h"

#include <spdlog/spdlog.h>


size_t UPackagePool::defaultCapacity = 64;
size_t UPackagePool::minCapacity = 16;

float UPackagePool::expanseRate = 0.3f;
float UPackagePool::expanseScale = 1.f;

float UPackagePool::collectRate = 1.f;
float UPackagePool::collectScale = 0.7f;

std::function<IPackage*()> UPackagePool::createPackage = nullptr;
std::function<void(IPackage*)> UPackagePool::initPackage = nullptr;

IPackage *DefaultPackage() {
    return new FPackage();
}

void PackageDefaultInit(IPackage *pkg) {
    const auto tmp = dynamic_cast<FPackage *>(pkg);
    if (tmp == nullptr)
        return;

    const auto &cfg = GetServerConfig();

    if (cfg["package"].IsNull())
        return;

    if (!cfg["package"]["magic"].IsNull())
        tmp->SetMagic(cfg["package"]["magic"].as<uint32_t>());

    if (!cfg["package"]["version"].IsNull())
        tmp->SetVersion(cfg["package"]["version"].as<uint32_t>());

    if (!cfg["package"]["method"].IsNull()) {
        if (const auto method = cfg["package"]["method"].as<std::string>(); method == "LineBased")
            tmp->ChangeMethod(ECodecMethod::LINE_BASED);
        else if (method == "Protobuf")
            tmp->ChangeMethod(ECodecMethod::PROTOBUF);
    }
}

UPackagePool::UPackagePool(const size_t capacity) {
    for (size_t i = 0; i < capacity; i++) {
        if (createPackage)
            queue_.emplace(createPackage());
        else
            queue_.emplace(DefaultPackage());
    }
}

UPackagePool::~UPackagePool() {
    while (!queue_.empty()) {
        const auto pkg = queue_.front();
        queue_.pop();
        delete pkg;
    }
}

IPackage *UPackagePool::Acquire() {
    Expanse();

    IPackage *pkg = queue_.front();
    queue_.pop();
    useCount_++;

    if (initPackage)
        initPackage(pkg);
    else
        PackageDefaultInit(pkg);

    return pkg;
}

void UPackagePool::Recycle(IPackage *pkg) {
    if (pkg != nullptr) {
        pkg->Reset();
        queue_.push(pkg);
        useCount_--;
    }

    Collect();
}

void UPackagePool::LoadConfig(const YAML::Node &cfg) {
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

    spdlog::info("Package pool configuration loaded.");
}

void UPackagePool::SetDefaultCapacity(const size_t capacity) {
    defaultCapacity = capacity;
}

void UPackagePool::SetMinimumCapacity(const size_t capacity) {
    minCapacity = capacity;
}

void UPackagePool::SetExpanseRate(const float rate) {
    expanseRate = rate;
}

void UPackagePool::SetExpanseScale(const float scale) {
    expanseScale = scale;
}

void UPackagePool::SetCollectRate(const float rate) {
    collectRate = rate;
}

void UPackagePool::SetCollectScale(const float scale) {
    collectScale = scale;
}

void UPackagePool::SetPackageBuilder(const std::function<IPackage *()> &func) {
    createPackage = func;
}

void UPackagePool::SetPackageInitializer(const std::function<void(IPackage *)> &func) {
    initPackage = func;
}

void UPackagePool::Expanse() {
    if (useCount_ == 0)
        return;

    if (std::floor(queue_.size() / useCount_) >= expanseRate)
        return;

    const auto num = static_cast<size_t>(std::ceil(static_cast<float>(useCount_) * expanseScale));
    spdlog::trace("{} - Pool rest: {}, current using: {}, expand number: {}.", __func__, queue_.size(), useCount_, num);

    for (size_t i = 0; i < num; i++)
        if (createPackage)
            queue_.emplace(createPackage());
        else
            queue_.emplace(DefaultPackage());
}

void UPackagePool::Collect() {
    const auto now = std::chrono::steady_clock::now();

    // 不要太频繁
    if (now - collectTime_ < std::chrono::seconds(3))
        return;

    if (queue_.size() <= minCapacity || std::floor(queue_.size() / (useCount_ == 0 ? 1 : useCount_)) < collectRate)
        return;

    collectTime_ = now;

    const auto num = static_cast<size_t>(std::ceil(static_cast<float>(queue_.size()) * collectScale));
    spdlog::trace("{} - Pool rest: {}, current using: {}, collect number: {}.", __func__, queue_.size(), useCount_, num);

    for (size_t i = 0; i < num && queue_.size() > minCapacity; i++) {
        const auto pkg = queue_.front();
        queue_.pop();
        delete pkg;
    }
}
