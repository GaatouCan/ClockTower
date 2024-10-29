#include "ProtocolSystem.h"
#include "../impl/ProtocolHandlerImpl.h"
#include "../../player/Player.h"

namespace base {
    void ProtocolSystem::Init() {
        SetHandler<ProtocolHandlerImpl>();
        RegisterProtocol();
    }

    awaitable<void> ProtocolSystem::OnReadPackage(const std::shared_ptr<UConnection> &conn, IPackage *pkg) const {
        if (handler_ != nullptr) {
            co_await handler_->Execute(conn, pkg);
        }
    }
} // base