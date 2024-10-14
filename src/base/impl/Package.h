#pragma once

#include "../IPackage.h"

#include <vector>
#include <sstream>

namespace base {
    static constexpr uint32_t MAGIC_NUMBER = 168712;
    static constexpr uint32_t PACKAGE_VERSION = 20241013;
    static constexpr uint32_t UNAVAILABLE_PACKAGE_ID = 1000;

    enum class ECodecMethod : uint16_t {
        LINE_BASED,
        PROTOBUF
    };

    static constexpr auto DEFAULT_ENCODING_METHOD = ECodecMethod::PROTOBUF;

    class Package final : public IPackage {

        friend class PackageCodecImpl;

        struct Header {
            uint32_t magic;
            uint32_t version;

            ECodecMethod method;
            uint16_t reserved;

            uint32_t id;
            size_t size;
        };

        Header header_;
        std::vector<uint8_t> data_;

    public:
        Package();
        ~Package() override;

        Package(const Package &rhs);
        Package(Package &&rhs) noexcept;

        Package &operator=(const Package &rhs);
        Package &operator=(Package &&rhs) noexcept;

        Package(uint32_t id, std::string_view data);
        Package(uint32_t id, const std::stringstream &ss);

        void reset() override;
        void invalid() override;
        [[nodiscard]] bool isAvailable() const override;

        Package &changeMethod(ECodecMethod method);
        Package &setPackageId(uint32_t id);

        Package &setData(std::string_view data);
        Package &setData(const std::stringstream &ss);

        [[nodiscard]] ECodecMethod method() const;
        [[nodiscard]] uint32_t id() const;

        [[nodiscard]] size_t dataLength() const;

        [[nodiscard]] std::string data() const;
        [[nodiscard]] const std::vector<uint8_t> &rawData() const;

        static constexpr size_t headerSize = sizeof(Header);
    };
} // base
