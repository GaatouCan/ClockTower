#pragma once

#include "common.h"


class ISubSystem {

public:
    ISubSystem() = default;
    virtual ~ISubSystem() = default;

    DISABLE_COPY_MOVE(ISubSystem)

    virtual void Init() = 0;
    [[nodiscard]] virtual const char *GetSystemName() const = 0;
};

template<class T>
concept SYSTEM_TYPE = std::derived_from<T, ISubSystem>;
