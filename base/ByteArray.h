#pragma once

#include <vector>
#include <utility>

struct FByteArray {
    std::vector<uint8_t> mBytes;

    FByteArray() = default;

    explicit FByteArray(const std::vector<uint8_t> &bytes)
        : mBytes(bytes) {}

    explicit operator std::vector<uint8_t>() const {
        return mBytes;
    }

    [[nodiscard]] size_t size() const {
        return mBytes.size();
    }

    [[nodiscard]] const uint8_t *data() const {
        return mBytes.data();
    }

    auto begin() -> decltype(mBytes)::iterator {
        return mBytes.begin();
    }

    auto end() -> decltype(mBytes)::iterator {
        return mBytes.end();
    }

    [[nodiscard]] auto begin() const -> decltype(mBytes)::const_iterator {
        return mBytes.begin();
    }

    [[nodiscard]] auto end() const -> decltype(mBytes)::const_iterator {
        return mBytes.end();
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
        const bool ret = mBytes.size() >= sizeof(std::remove_pointer_t<T>);
        const auto size = ret ?  sizeof(std::remove_pointer_t<T>) : mBytes.size();

        memset(target, 0, size);
        memcpy(target, mBytes.data(), size);

        return ret;
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
    const bool ret = bytes.size() >= sizeof(std::remove_pointer_t<T>);
    const auto size = ret ? sizeof(std::remove_pointer_t<T>) : bytes.size();

    memset(data, 0, size);
    memcpy(data, bytes.data(), size);

    return ret;
}
