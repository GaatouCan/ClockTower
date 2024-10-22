#pragma once

#include "../../protobuf/ProtoType.generated.h"

#include <asio.hpp>

namespace base {

    using protocol::ProtoType;

    template<typename FUNCTOR>
    class TProtocolHandler {
    public:
        virtual ~TProtocolHandler() = default;
        virtual asio::awaitable<void> Execute(const std::shared_ptr<class Connection> &, class IPackage *) = 0;

        FUNCTOR Find(ProtoType type) const {
            if (const auto it = protoMap_.find(type); it != protoMap_.end()) {
                return it->second;
            }
            return nullptr;
        }

        void Register(ProtoType type, FUNCTOR func) {
            protoMap_[type] = func;
        }

    private:
        std::unordered_map<ProtoType, FUNCTOR> protoMap_;
    };
}
