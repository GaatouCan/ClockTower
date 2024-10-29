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

    class FPackage final : public IPackage {

        friend class UPackageCodecImpl;

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
        FPackage();
        ~FPackage() override;

        FPackage(const FPackage &rhs);
        FPackage(FPackage &&rhs) noexcept;

        FPackage &operator=(const FPackage &rhs);
        FPackage &operator=(FPackage &&rhs) noexcept;

        FPackage(uint32_t id, std::string_view data);
        FPackage(uint32_t id, const std::stringstream &ss);

        void Reset() override;
        void Invalid() override;
        [[nodiscard]] bool IsAvailable() const override;

        FPackage &SetPackageID(uint32_t id);
        [[nodiscard]] uint32_t GetID() const override;

        FPackage &SetData(std::string_view data);
        FPackage &SetData(const std::stringstream &ss);

        FPackage &SetMagic(uint32_t magic);
        [[nodiscard]] uint32_t GetMagic() const;

        FPackage &SetVersion(uint32_t version);
        [[nodiscard]] uint32_t GetVersion() const;

        FPackage &ChangeMethod(CodecMethod method);
        [[nodiscard]] CodecMethod GetMethod() const;

        [[nodiscard]] size_t GetDataLength() const;

        [[nodiscard]] std::string GetData() const;
        [[nodiscard]] const std::vector<uint8_t> &GetRawData() const;

        static constexpr size_t headerSize = sizeof(Header);
    };
} // base
