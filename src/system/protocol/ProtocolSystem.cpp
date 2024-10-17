#include "ProtocolSystem.h"
#include "../impl/ProtocolHandlerImpl.h"
#include "../../player/Player.h"

namespace base {
    void ProtocolSystem::Init() {
        SetHandler<ProtocolHandlerImpl>();
        RegisterProtocol();
    }

    void ProtocolSystem::OnReadPackage(const std::shared_ptr<Connection> &conn, IPackage *pkg) const {
        if (handler_ != nullptr) {
            handler_->Execute(conn, pkg);
        }
    }
} // base