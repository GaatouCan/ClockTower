#pragma once

#include <functional>


struct IEventParam {
    virtual ~IEventParam() = default;
};


using EventListener = std::function<void(IEventParam *)>;
