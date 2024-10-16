#pragma once

#include <concepts>

namespace base {
    class IPackage {
    public:
        virtual ~IPackage() = default;

        [[nodiscard]] virtual uint32_t GetID() const { return 0; }

        /**
         * Reset all content in package
         */
        virtual void Reset() = 0;

        /**
         * Make package unavailable
         */
        virtual void Invalid() = 0;

        /**
         * Judge whether the package is valid
         * @return bool
         */
        [[nodiscard]] virtual bool IsAvailable() const = 0;
    };

    template<typename T>
    concept PACKAGE_TYPE = std::derived_from<T, IPackage>;
}
