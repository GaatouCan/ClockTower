#pragma once

#include "../../base/ISubSystem.h"
#include "../TProtocolHandler.h"
#include "../../base/impl/Package.h"

#include <concepts>

class Player;

namespace base {

    class Connection;
    class Package;

    using ProtoFunctor = void(*)(const std::shared_ptr<Player>&, Package *);

    class ProtocolSystem final : public ISubSystem {

        SUB_SYSTEM_BODY(ProtocolSystem)

        void Init() override;

        void RegisterProtocol() const;

    public:
        template<typename T>
        requires std::derived_from<T, TProtocolHandler<ProtoFunctor>>
        void SetHandler() {
            if (handler_ != nullptr)
                handler_.reset();

            handler_ = std::make_unique<T>();
        }

        void OnReadPackage(const std::shared_ptr<Connection> &conn, IPackage *pkg) const;

    private:
        std::unique_ptr<TProtocolHandler<ProtoFunctor>> handler_;
    };

#define REGISTER_PROTOCOL(proto) \
    handler_->Register(ProtoType::proto, &proto);

} // base
