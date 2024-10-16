#pragma once

#include <concepts>

namespace base {
    class IPackage {
    public:
        virtual ~IPackage() = default;

        [[nodiscard]] virtual uint32_t id() const { return 0; }

        /**
         * Reset all content in package
         */
        virtual void reset() = 0;

        /**
         * Make package unavailable
         */
        virtual void invalid() = 0;

        /**
         * Judge whether the package is valid
         * @return bool
         */
        [[nodiscard]] virtual bool isAvailable() const = 0;
    };

    template<typename T>
    concept PACKAGE_TYPE = std::derived_from<T, IPackage>;
}
