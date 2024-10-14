#pragma once

#include "../base/IPackage.h"
#include <asio.hpp>

namespace base {

    enum class ProtoType : uint32_t {

    };

    template<typename  T, PACKAGE_TYPE PT>
    class TProtocolHandler {

        using functor = asio::awaitable<void>(*)(const std::shared_ptr<T> &, PT *);
        std::unordered_map<ProtoType, functor> functorMap_;

    public:
        virtual ~TProtocolHandler() = default;

        virtual asio::awaitable<void> execute(const std::shared_ptr<T> &, PT *) = 0;

        functor find(ProtoType type) {
            auto it = functorMap_.find(type);
            if (it != functorMap_.end())
                return it->second;
            return nullptr;
        }

        void registerProtocol(ProtoType type, functor func) {
            functorMap_.insert_or_assign(type, func);
        }
    };
}
