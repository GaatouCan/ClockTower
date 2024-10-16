#include "Package.h"

namespace base {
    Package::Package()
        : header_() {
        memset(&header_, 0, sizeof(header_));
    }

    Package::~Package() = default;

    Package::Package(const Package &rhs) : Package() {
        if (this != &rhs) {
            memcpy(&header_, &rhs.header_, sizeof(header_));

            data_.reserve(rhs.data_.size());
            memcpy(data_.data(), rhs.data_.data(), rhs.data_.size());

            header_.size = data_.size();
        }
    }

    Package::Package(Package &&rhs) noexcept : Package() {
        if (this != &rhs) {
            memcpy(&header_, &rhs.header_, sizeof(header_));

            data_ = std::move(rhs.data_);
            header_.size = data_.size();
        }
    }

    Package &Package::operator=(const Package &rhs) {
        if (this != &rhs) {
            memcpy(&header_, &rhs.header_, sizeof(header_));

            data_.reserve(rhs.data_.size());
            memcpy(data_.data(), rhs.data_.data(), rhs.data_.size());

            header_.size = data_.size();
        }
        return *this;
    }

    Package &Package::operator=(Package &&rhs) noexcept {
        if (this != &rhs) {
            memcpy(&header_, &rhs.header_, sizeof(header_));

            data_ = std::move(rhs.data_);
            header_.size = data_.size();
        }
        return *this;
    }

    Package::Package(const uint32_t id, const std::string_view data)
        : Package() {
        data_.resize(data.size());
        memcpy(data_.data(), data.data(), data.size());
        header_.id = id;
        header_.size = data_.size();
    }

    Package::Package(const uint32_t id, const std::stringstream &ss)
        : Package(id, ss.str()) {
    }

    void Package::Reset() {
        memset(&header_, 0, sizeof(header_));

        data_.clear();
        data_.shrink_to_fit();
    }

    void Package::Invalid() {
        header_.id = UNAVAILABLE_PACKAGE_ID;
    }

    bool Package::IsAvailable() const {
        return header_.id > UNAVAILABLE_PACKAGE_ID;
    }

    Package &Package::ChangeMethod(const CodecMethod method) {
        header_.method = method;
        return *this;
    }

    Package &Package::SetPackageID(const uint32_t id) {
        header_.id = id;
        return *this;
    }

    Package &Package::SetData(const std::string_view data) {
        data_.resize(data.size());
        memcpy(data_.data(), data.data(), data.size());
        header_.size = data_.size();
        return *this;
    }

    Package &Package::SetData(const std::stringstream &ss) {
        return SetData(ss.str());
    }

    Package & Package::SetMagic(const uint32_t magic) {
        header_.magic = magic;
        return *this;
    }

    Package & Package::SetVersion(const uint32_t version) {
        header_.version = version;
        return *this;
    }

    uint32_t Package::GetMagic() const {
        return header_.magic;
    }

    uint32_t Package::GetVersion() const {
        return header_.version;
    }

    CodecMethod Package::GetMethod() const {
        return header_.method;
    }

    uint32_t Package::GetID() const {
        return header_.id;
    }

    size_t Package::GetDataLength() const {
        return data_.size();
    }

    std::string Package::GetData() const {
        return {data_.begin(), data_.end()};
    }

    const std::vector<uint8_t> &Package::GetRawData() const {
        return data_;
    }
} // base
