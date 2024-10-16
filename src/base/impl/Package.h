#pragma once

#include "../IPackage.h"

#include <vector>
#include <sstream>

namespace base {

    static constexpr uint32_t UNAVAILABLE_PACKAGE_ID = 1000;

    enum class CodecMethod : uint16_t {
        LINE_BASED,
        PROTOBUF
    };

    static constexpr auto DEFAULT_ENCODING_METHOD = CodecMethod::PROTOBUF;

    class Package final : public IPackage {

        friend class PackageCodecImpl;

        struct Header {
            uint32_t magic;
            uint32_t version;

            CodecMethod method;
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

        Package &setPackageId(uint32_t id);
        [[nodiscard]] uint32_t id() const override;

        Package &setData(std::string_view data);
        Package &setData(const std::stringstream &ss);

        Package &setMagic(uint32_t magic);
        [[nodiscard]] uint32_t magic() const;

        Package &setVersion(uint32_t version);
        [[nodiscard]] uint32_t version() const;

        Package &changeMethod(CodecMethod method);
        [[nodiscard]] CodecMethod method() const;

        [[nodiscard]] size_t dataLength() const;

        [[nodiscard]] std::string data() const;
        [[nodiscard]] const std::vector<uint8_t> &rawData() const;

        static constexpr size_t headerSize = sizeof(Header);
    };
} // base
