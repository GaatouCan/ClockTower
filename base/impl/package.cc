#include "package.h"

#ifdef __linux__
#include <cstring>
#endif


FPackage::FPackage()
    : mHeader() {
    memset(&mHeader, 0, sizeof(mHeader));
}

FPackage::~FPackage() = default;

FPackage::FPackage(const FPackage &rhs) : FPackage() {
    if (this != &rhs) {
        memcpy(&mHeader, &rhs.mHeader, sizeof(mHeader));

        mBytes.resize(rhs.mBytes.size());
        memcpy(mBytes.data(), rhs.mBytes.data(), rhs.mBytes.size());

        mHeader.mLength = mBytes.size();
    }
}

FPackage::FPackage(FPackage &&rhs) noexcept : FPackage() {
    if (this != &rhs) {
        memcpy(&mHeader, &rhs.mHeader, sizeof(mHeader));

        mBytes = std::move(rhs.mBytes);
        mHeader.mLength = mBytes.size();
    }
}

FPackage &FPackage::operator=(const FPackage &rhs) {
    if (this != &rhs) {
        memcpy(&mHeader, &rhs.mHeader, sizeof(mHeader));

        mBytes.resize(rhs.mBytes.size());
        memcpy(mBytes.data(), rhs.mBytes.data(), rhs.mBytes.size());

        mHeader.mLength = mBytes.size();
    }
    return *this;
}

FPackage &FPackage::operator=(FPackage &&rhs) noexcept {
    if (this != &rhs) {
        memcpy(&mHeader, &rhs.mHeader, sizeof(mHeader));

        mBytes = std::move(rhs.mBytes);
        mHeader.mLength = mBytes.size();
    }
    return *this;
}

FPackage::FPackage(const uint32_t id, const std::string_view data)
    : FPackage() {
    mBytes.resize(data.size());
    memcpy(mBytes.data(), data.data(), data.size());
    mHeader.mID = id;
    mHeader.mLength = mBytes.size();
}

FPackage::FPackage(const uint32_t id, const std::stringstream &ss)
    : FPackage(id, ss.str()) {
}

void FPackage::Reset() {
    memset(&mHeader, 0, sizeof(mHeader));

    mBytes.clear();
    mBytes.shrink_to_fit();
}

void FPackage::Invalid() {
    mHeader.mID = kUnavailablePackageId;
}

bool FPackage::IsAvailable() const {
    return mHeader.mID > kUnavailablePackageId;
}

FPackage &FPackage::ChangeMethod(const ECodecMethod method) {
    mHeader.mMethod = method;
    return *this;
}

FPackage &FPackage::SetPackageID(const uint32_t id) {
    mHeader.mID = id;
    return *this;
}

FPackage &FPackage::SetData(const std::string_view data) {
    mBytes.resize(data.size());
    memcpy(mBytes.data(), data.data(), data.size());
    mHeader.mLength = mBytes.size();
    return *this;
}

FPackage &FPackage::SetData(const std::stringstream &ss) {
    return SetData(ss.str());
}

FPackage &FPackage::SetMagic(const uint32_t magic) {
    mHeader.mMagic = magic;
    return *this;
}

FPackage &FPackage::SetVersion(const uint32_t version) {
    mHeader.mVersion = version;
    return *this;
}

uint32_t FPackage::GetMagic() const {
    return mHeader.mMagic;
}

uint32_t FPackage::GetVersion() const {
    return mHeader.mVersion;
}

ECodecMethod FPackage::GetMethod() const {
    return mHeader.mMethod;
}

uint32_t FPackage::GetID() const {
    return mHeader.mID;
}

void FPackage::CopyFromOther(IPackage *other) {
    if (const auto tmp = dynamic_cast<FPackage*>(other); tmp != nullptr) {
        *this = *tmp;
    }
}

size_t FPackage::GetDataLength() const {
    return mBytes.size();
}

std::string FPackage::GetData() const {
    return {mBytes.begin(), mBytes.end()};
}

const std::vector<uint8_t> &FPackage::GetRawData() const {
    return mBytes;
}
