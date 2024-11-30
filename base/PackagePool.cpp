#include "PackagePool.h"
#include "impl/Package.h"
#include "system/config/ConfigSystem.h"

#include <spdlog/spdlog.h>


size_t UPackagePool::sDefaultCapacity = 64;
size_t UPackagePool::sMinCapacity = 16;

float UPackagePool::sExpanseRate = 0.3f;
float UPackagePool::sExpanseScale = 1.f;

float UPackagePool::sCollectRate = 1.f;
float UPackagePool::sCollectScale = 0.7f;

std::function<IPackage*()> UPackagePool::sCreatePackage = nullptr;
std::function<void(IPackage *)> UPackagePool::sInitPackage = nullptr;

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

UPackagePool::UPackagePool(const size_t capacity)
    : mUseCount(0) {
    for (size_t i = 0; i < capacity; i++) {
        IPackage *pkg = nullptr;
        if (sCreatePackage)
            pkg = sCreatePackage();
        else
            pkg = DefaultPackage();

        if (pkg != nullptr) {
            pkg->SetOwnerPool(this);
            mQueue.push(pkg);
        }
    }
}

UPackagePool::~UPackagePool() {
    while (!mQueue.empty()) {
        const auto pkg = mQueue.front();
        mQueue.pop();
        delete pkg;
    }
}

size_t UPackagePool::GetCapacity() const {
    return mUseCount + mQueue.size();
}

IPackage *UPackagePool::Acquire() {
    Expanse();

    IPackage *pkg = mQueue.front();
    mQueue.pop();
    mUseCount++;

    if (sInitPackage)
        std::invoke(sInitPackage, pkg);
    else
        PackageDefaultInit(pkg);

    return pkg;
}

void UPackagePool::Recycle(IPackage *pkg) {
    if (pkg != nullptr) {
        pkg->Reset();
        mQueue.push(pkg);
        mUseCount--;
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

    spdlog::info("Package Pool Configuration Loaded Successfully.");
}

void UPackagePool::SetDefaultCapacity(const size_t capacity) {
    sDefaultCapacity = capacity;
}

void UPackagePool::SetMinimumCapacity(const size_t capacity) {
    sMinCapacity = capacity;
}

void UPackagePool::SetExpanseRate(const float rate) {
    sExpanseRate = rate;
}

void UPackagePool::SetExpanseScale(const float scale) {
    sExpanseScale = scale;
}

void UPackagePool::SetCollectRate(const float rate) {
    sCollectRate = rate;
}

void UPackagePool::SetCollectScale(const float scale) {
    sCollectScale = scale;
}

void UPackagePool::SetPackageBuilder(const std::function<IPackage *()> &func) {
    sCreatePackage = func;
}

void UPackagePool::SetPackageInitializer(const std::function<void(IPackage *)> &func) {
    sInitPackage = func;
}

void UPackagePool::Expanse() {
    if (mUseCount == 0)
        return;

    if (std::floor(mQueue.size() / GetCapacity()) >= sExpanseRate)
        return;

    const auto num = static_cast<size_t>(std::ceil(static_cast<float>(GetCapacity()) * sExpanseScale));
    spdlog::trace("{} - Pool Rest[{}], Current Using[{}], Expand Number[{}].", __FUNCTION__, mQueue.size(), mUseCount, num);

    for (size_t i = 0; i < num; i++) {
        IPackage *pkg = nullptr;
        if (sCreatePackage)
            pkg = sCreatePackage();
        else
            pkg = DefaultPackage();

        if (pkg != nullptr) {
            pkg->SetOwnerPool(this);
            mQueue.push(pkg);
        }
    }
}

void UPackagePool::Collect() {
    const auto now = std::chrono::steady_clock::now();

    // 不要太频繁
    if (now - mCollectTime < std::chrono::seconds(3))
        return;

    if (mQueue.size() <= sMinCapacity || std::floor(mQueue.size() / GetCapacity()) < sCollectRate)
        return;

    mCollectTime = now;

    const auto num = static_cast<size_t>(std::ceil(static_cast<float>(GetCapacity()) * sCollectScale));
    spdlog::trace("{} - Pool Rest[{}], Current Using[{}], Collect Number[{}].", __FUNCTION__, mQueue.size(), mUseCount, num);

    for (size_t i = 0; i < num && mQueue.size() > sMinCapacity; i++) {
        const auto pkg = mQueue.front();
        mQueue.pop();
        delete pkg;
    }
}
