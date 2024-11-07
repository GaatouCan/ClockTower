#include "Package.h"


FPackage::FPackage()
    : mHeader() {
    memset(&mHeader, 0, sizeof(mHeader));
}

FPackage::~FPackage() = default;

FPackage::FPackage(const FPackage &rhs) : FPackage() {
    if (this != &rhs) {
        memcpy(&mHeader, &rhs.mHeader, sizeof(mHeader));

        mData.reserve(rhs.mData.size());
        memcpy(mData.data(), rhs.mData.data(), rhs.mData.size());

        mHeader.size = mData.size();
    }
}

FPackage::FPackage(FPackage &&rhs) noexcept : FPackage() {
    if (this != &rhs) {
        memcpy(&mHeader, &rhs.mHeader, sizeof(mHeader));

        mData = std::move(rhs.mData);
        mHeader.size = mData.size();
    }
}

FPackage &FPackage::operator=(const FPackage &rhs) {
    if (this != &rhs) {
        memcpy(&mHeader, &rhs.mHeader, sizeof(mHeader));

        mData.reserve(rhs.mData.size());
        memcpy(mData.data(), rhs.mData.data(), rhs.mData.size());

        mHeader.size = mData.size();
    }
    return *this;
}

FPackage &FPackage::operator=(FPackage &&rhs) noexcept {
    if (this != &rhs) {
        memcpy(&mHeader, &rhs.mHeader, sizeof(mHeader));

        mData = std::move(rhs.mData);
        mHeader.size = mData.size();
    }
    return *this;
}

FPackage::FPackage(const uint32_t id, const std::string_view data)
    : FPackage() {
    mData.resize(data.size());
    memcpy(mData.data(), data.data(), data.size());
    mHeader.id = id;
    mHeader.size = mData.size();
}

FPackage::FPackage(const uint32_t id, const std::stringstream &ss)
    : FPackage(id, ss.str()) {
}

void FPackage::Reset() {
    memset(&mHeader, 0, sizeof(mHeader));

    mData.clear();
    mData.shrink_to_fit();
}

void FPackage::Invalid() {
    mHeader.id = kUnavailablePackageId;
}

bool FPackage::IsAvailable() const {
    return mHeader.id > kUnavailablePackageId;
}

FPackage &FPackage::ChangeMethod(const ECodecMethod method) {
    mHeader.method = method;
    return *this;
}

FPackage &FPackage::SetPackageID(const uint32_t id) {
    mHeader.id = id;
    return *this;
}

FPackage &FPackage::SetData(const std::string_view data) {
    mData.resize(data.size());
    memcpy(mData.data(), data.data(), data.size());
    mHeader.size = mData.size();
    return *this;
}

FPackage &FPackage::SetData(const std::stringstream &ss) {
    return SetData(ss.str());
}

FPackage &FPackage::SetMagic(const uint32_t magic) {
    mHeader.magic = magic;
    return *this;
}

FPackage &FPackage::SetVersion(const uint32_t version) {
    mHeader.version = version;
    return *this;
}

uint32_t FPackage::GetMagic() const {
    return mHeader.magic;
}

uint32_t FPackage::GetVersion() const {
    return mHeader.version;
}

ECodecMethod FPackage::GetMethod() const {
    return mHeader.method;
}

uint32_t FPackage::GetID() const {
    return mHeader.id;
}

size_t FPackage::GetDataLength() const {
    return mData.size();
}

std::string FPackage::GetData() const {
    return {mData.begin(), mData.end()};
}

const std::vector<uint8_t> &FPackage::GetRawData() const {
    return mData;
}
