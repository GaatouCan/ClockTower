#pragma once

#include "../../base/ISubSystem.h"
#include "../TProtocolHandler.h"
#include "../../base/impl/Package.h"

#include <concepts>

class Player;

namespace base {

    class Connection;
    class Package;

    using ProtoFunctor = awaitable<void>(*)(const std::shared_ptr<Player>&, Package *);

    class ProtocolSystem final : public ISubSystem {

        SUB_SYSTEM_BODY(ProtocolSystem)

        void init() override;

        void registerProtocol() const;

    public:
        template<typename T>
        requires std::derived_from<T, TProtocolHandler<ProtoFunctor>>
        void setHandler() {
            if (handler_ != nullptr)
                handler_.reset();

            handler_ = std::make_unique<T>();
        }

        awaitable<void> onReadPackage(const std::shared_ptr<Connection> &conn, IPackage *pkg) const;

    private:
        std::unique_ptr<TProtocolHandler<ProtoFunctor>> handler_;
    };

#define REGISTER_PROTOCOL(proto) \
    handler_->registerProtocol(ProtoType::proto, &proto);

} // base
