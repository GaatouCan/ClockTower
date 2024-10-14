#pragma once

#include <concepts>

namespace base {
    class IPackage {
    public:
        virtual ~IPackage() = default;

        /**
         * Reset the package content
         */
        virtual void reset() = 0;

        virtual void invalid() = 0;
        [[nodiscard]] virtual bool isAvailable() const = 0;
    };

    template<typename T>
    concept PACKAGE_TYPE = std::derived_from<T, IPackage>;
}
