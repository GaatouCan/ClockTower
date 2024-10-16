#pragma once

#include "../common/common.h"

namespace base {
    class ISubSystem {

        friend class GameWorld;

    protected:
        ISubSystem() = default;
        virtual ~ISubSystem() = default;

        virtual void Init() = 0;

    public:
        DISABLE_COPY_MOVE(ISubSystem)
        [[nodiscard]] virtual const char* GetSystemName() const = 0;
    };

    template<class T>
    concept SYSTEM_TYPE = std::derived_from<T, ISubSystem>;

#define SUB_SYSTEM_BODY(sys) \
    friend GameWorld; \
public: \
    [[nodiscard]] constexpr const char *GetSystemName() const override { \
        return #sys; \
    } \
private:

}
