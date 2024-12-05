#include <spdlog/spdlog.h>
#include <asio.hpp>
#include <concepts>
#include <string>


struct FByteArray {
    std::vector<uint8_t> mBytes;

    FByteArray() = default;

    FByteArray(const std::vector<uint8_t> &bytes)
        : mBytes(bytes) {}

    operator std::vector<uint8_t>() const {
        return mBytes;
    }

    [[nodiscard]] size_t Length() const {
        return mBytes.size();
    }

    template<typename T>
    static constexpr bool IsPODType = std::is_pointer_v<T> ?
        std::is_trivial_v<std::remove_pointer_t<T>> && std::is_standard_layout_v<std::remove_pointer_t<T>> :
        std::is_trivial_v<T> && std::is_standard_layout_v<T>;

    template<typename T>
    requires IsPODType<T>
    void CastFromType(T source) {
        if constexpr (std::is_pointer_v<T>) {
            constexpr auto size = sizeof(std::remove_pointer_t<T>);
            mBytes.resize(size);
            memcpy(mBytes.data(), source, size);
        } else {
            mBytes.resize(sizeof(T));
            memcpy(mBytes.data(), &source, sizeof(T));
        }
    }

    template<typename T>
    requires IsPODType<T>
    bool CastToType(T *target) const {
        if (mBytes.size() < sizeof(std::remove_pointer_t<T>))
            return false;

        constexpr auto size = sizeof(std::remove_pointer_t<T>);

        memset(target, 0, size);
        memcpy(target, mBytes.data(), size);

        return true;
    }

    template<typename T>
    requires IsPODType<T>
    static FByteArray FromType(T data) {
        FByteArray bytes;
        bytes.CastFromType(std::forward<T>(data));
        return bytes;
    }
};

template<typename T>
requires FByteArray::IsPODType<T>
std::vector<uint8_t> PODToByteArray(T data) {
    std::vector<uint8_t> bytes;
    if constexpr (std::is_pointer_v<T>) {
        bytes.resize(sizeof(std::remove_pointer_t<T>));
        memcpy(bytes.data(), data, sizeof(std::remove_pointer_t<T>));
    } else {
        bytes.resize(sizeof(data));
        memcpy(bytes.data(), &data, sizeof(data));
    }
    return bytes;
}

template<typename T>
requires FByteArray::IsPODType<T>
bool ByteArrayToPOD(const std::vector<uint8_t> &bytes, T *data) {
    if (bytes.size() < sizeof(std::remove_pointer_t<T>))
        return false;

    memset(data, 0, sizeof(std::remove_pointer_t<T>));
    memcpy(data, bytes.data(), sizeof(std::remove_pointer_t<T>));

    return true;
}
struct FDemo {
    uint32_t hello;
    uint32_t world;
    bool bFlag;
    char name[20];
};

int main() {
    spdlog::info("Hello World!");

    spdlog::info("{}", std::is_standard_layout_v<std::vector<uint8_t> >);

    FDemo demo{
        37, 42, true, "Gaatou"
    };

    const FByteArray bytes = FByteArray::FromType(&demo);
    spdlog::info("{}", bytes.Length());

    FDemo tmp{};
    if (bytes.CastToType(&tmp))
        spdlog::info("{} - {} - {} - {}", tmp.hello, tmp.world, tmp.bFlag, tmp.name);


    return 0;
}
