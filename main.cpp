#include <spdlog/spdlog.h>
#include <asio.hpp>
#include <concepts>
#include <string>

template<typename T>
consteval bool IsPODType() {
    if constexpr (std::is_pointer_v<T>) {
        if constexpr (std::is_trivial_v<std::remove_pointer_t<T>> && std::is_standard_layout_v<std::remove_pointer_t<T>>)
            return true;
        else
            return false;
    } else {
        if constexpr (std::is_trivial_v<T> && std::is_standard_layout_v<T>)
            return true;
        else
            return false;
    }
}

template<typename T>
consteval bool CheckPODType(T) {
    return IsPODType<T>();
}


struct FDemo {
    uint32_t hello;
    uint32_t world;
    char name[20];
};

template<typename T>
std::vector<uint8_t> PODToByteArray(T data) {
    if constexpr (!IsPODType<T>()) {
        return {};
    }
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
bool ByteArrayToPOD(const std::vector<uint8_t>& bytes, T *data) {
    if constexpr (!IsPODType<T>())
        return false;

    if (bytes.size() < sizeof(std::remove_pointer_t<T>))
        return false;

    memset(data, 0, sizeof(std::remove_pointer_t<T>));
    memcpy(data, bytes.data(), sizeof(std::remove_pointer_t<T>));

    return true;
}

int main() {
    spdlog::info("Hello World!");

    spdlog::info("{}", IsPODType<FDemo>());

    FDemo demo {
        37, 42, "Gaatou"
    };

    const auto bytes = PODToByteArray(&demo);
    spdlog::info("{}", bytes.size());

    FDemo tmp{};
    if (ByteArrayToPOD(bytes, &tmp)) {
        spdlog::info("{} - {} - {}", tmp.hello, tmp.world, tmp.name);
    }


    return 0;
}
