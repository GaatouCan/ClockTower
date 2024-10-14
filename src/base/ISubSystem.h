#pragma once

#include "../common/common.h"

namespace base {
    class ISubSystem {

        friend class GameWorld;

    protected:
        ISubSystem() = default;
        virtual ~ISubSystem() = default;

        virtual void init() = 0;

    public:
        DISABLE_COPY_MOVE(ISubSystem)
        [[nodiscard]] virtual const char* name() const = 0;
    };

    template<class T>
    concept SYSTEM_TYPE = std::derived_from<T, ISubSystem>;

#define SUB_SYSTEM_BODY(sys) \
    friend GameWorld; \
public: \
    [[nodiscard]] constexpr const char *name() const override { \
        return #sys; \
    } \
private:

}
