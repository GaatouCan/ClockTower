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

        void Reset() override;
        void Invalid() override;
        [[nodiscard]] bool IsAvailable() const override;

        Package &SetPackageID(uint32_t id);
        [[nodiscard]] uint32_t GetID() const override;

        Package &SetData(std::string_view data);
        Package &SetData(const std::stringstream &ss);

        Package &SetMagic(uint32_t magic);
        [[nodiscard]] uint32_t GetMagic() const;

        Package &SetVersion(uint32_t version);
        [[nodiscard]] uint32_t GetVersion() const;

        Package &ChangeMethod(CodecMethod method);
        [[nodiscard]] CodecMethod GetMethod() const;

        [[nodiscard]] size_t GetDataLength() const;

        [[nodiscard]] std::string GetData() const;
        [[nodiscard]] const std::vector<uint8_t> &GetRawData() const;

        static constexpr size_t headerSize = sizeof(Header);
    };
} // base
