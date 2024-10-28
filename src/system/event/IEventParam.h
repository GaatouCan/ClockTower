#pragma once

#include <functional>

namespace base {
    struct IEventParam {
        virtual ~IEventParam() = default;
    };
}

using EventListener = std::function<void(base::IEventParam *)>;