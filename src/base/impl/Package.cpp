#include "Package.h"

namespace base {
    FPackage::FPackage()
        : header_() {
        memset(&header_, 0, sizeof(header_));
    }

    FPackage::~FPackage() = default;

    FPackage::FPackage(const FPackage &rhs) : FPackage() {
        if (this != &rhs) {
            memcpy(&header_, &rhs.header_, sizeof(header_));

            data_.reserve(rhs.data_.size());
            memcpy(data_.data(), rhs.data_.data(), rhs.data_.size());

            header_.size = data_.size();
        }
    }

    FPackage::FPackage(FPackage &&rhs) noexcept : FPackage() {
        if (this != &rhs) {
            memcpy(&header_, &rhs.header_, sizeof(header_));

            data_ = std::move(rhs.data_);
            header_.size = data_.size();
        }
    }

    FPackage &FPackage::operator=(const FPackage &rhs) {
        if (this != &rhs) {
            memcpy(&header_, &rhs.header_, sizeof(header_));

            data_.reserve(rhs.data_.size());
            memcpy(data_.data(), rhs.data_.data(), rhs.data_.size());

            header_.size = data_.size();
        }
        return *this;
    }

    FPackage &FPackage::operator=(FPackage &&rhs) noexcept {
        if (this != &rhs) {
            memcpy(&header_, &rhs.header_, sizeof(header_));

            data_ = std::move(rhs.data_);
            header_.size = data_.size();
        }
        return *this;
    }

    FPackage::FPackage(const uint32_t id, const std::string_view data)
        : FPackage() {
        data_.resize(data.size());
        memcpy(data_.data(), data.data(), data.size());
        header_.id = id;
        header_.size = data_.size();
    }

    FPackage::FPackage(const uint32_t id, const std::stringstream &ss)
        : FPackage(id, ss.str()) {
    }

    void FPackage::Reset() {
        memset(&header_, 0, sizeof(header_));

        data_.clear();
        data_.shrink_to_fit();
    }

    void FPackage::Invalid() {
        header_.id = UNAVAILABLE_PACKAGE_ID;
    }

    bool FPackage::IsAvailable() const {
        return header_.id > UNAVAILABLE_PACKAGE_ID;
    }

    FPackage &FPackage::ChangeMethod(const ECodecMethod method) {
        header_.method = method;
        return *this;
    }

    FPackage &FPackage::SetPackageID(const uint32_t id) {
        header_.id = id;
        return *this;
    }

    FPackage &FPackage::SetData(const std::string_view data) {
        data_.resize(data.size());
        memcpy(data_.data(), data.data(), data.size());
        header_.size = data_.size();
        return *this;
    }

    FPackage &FPackage::SetData(const std::stringstream &ss) {
        return SetData(ss.str());
    }

    FPackage & FPackage::SetMagic(const uint32_t magic) {
        header_.magic = magic;
        return *this;
    }

    FPackage & FPackage::SetVersion(const uint32_t version) {
        header_.version = version;
        return *this;
    }

    uint32_t FPackage::GetMagic() const {
        return header_.magic;
    }

    uint32_t FPackage::GetVersion() const {
        return header_.version;
    }

    ECodecMethod FPackage::GetMethod() const {
        return header_.method;
    }

    uint32_t FPackage::GetID() const {
        return header_.id;
    }

    size_t FPackage::GetDataLength() const {
        return data_.size();
    }

    std::string FPackage::GetData() const {
        return {data_.begin(), data_.end()};
    }

    const std::vector<uint8_t> &FPackage::GetRawData() const {
        return data_;
    }
} // base
