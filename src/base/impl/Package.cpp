#include "Package.h"

namespace base {
    Package::Package()
        : header_() {
        memset(&header_, 0, sizeof(header_));

        header_.magic = MAGIC_NUMBER;
        header_.version = PACKAGE_VERSION;
        header_.method = DEFAULT_ENCODING_METHOD;
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

    void Package::reset() {
        memset(&header_, 0, sizeof(header_));

        header_.magic = MAGIC_NUMBER;
        header_.version = PACKAGE_VERSION;
        header_.method = DEFAULT_ENCODING_METHOD;

        data_.clear();
        data_.shrink_to_fit();
    }

    void Package::invalid() {
        header_.id = UNAVAILABLE_PACKAGE_ID;
    }

    bool Package::isAvailable() const {
        return header_.id > UNAVAILABLE_PACKAGE_ID;
    }

    Package &Package::changeMethod(const ECodecMethod method) {
        header_.method = method;
        return *this;
    }

    Package &Package::setPackageId(const uint32_t id) {
        header_.id = id;
        return *this;
    }

    Package &Package::setData(const std::string_view data) {
        data_.resize(data.size());
        memcpy(data_.data(), data.data(), data.size());
        header_.size = data_.size();
        return *this;
    }

    Package &Package::setData(const std::stringstream &ss) {
        return setData(ss.str());
    }

    ECodecMethod Package::method() const {
        return header_.method;
    }

    uint32_t Package::id() const {
        return header_.id;
    }

    size_t Package::dataLength() const {
        return data_.size();
    }

    std::string Package::data() const {
        return {data_.begin(), data_.end()};
    }

    const std::vector<uint8_t> &Package::rawData() const {
        return data_;
    }
} // base
