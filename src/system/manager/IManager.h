#pragma once

#include "../../common/common.h"

namespace base {
    class IManager {

        asio::io_context &ctx_;

    public:
        IManager() = delete;

        explicit IManager(asio::io_context &ctx);
        virtual ~IManager() = default;

        DISABLE_COPY_MOVE(IManager)

        [[nodiscard]] asio::io_context &GetIOContext() const;
    };

    template<typename T>
    concept MANAGER_TYPE = std::derived_from<T, IManager>;

} // base
