#include "ProtocolHandlerImpl.h"
#include "../../base/Connection.h"

#include <spdlog/spdlog.h>

namespace base {
    awaitable<void> ProtocolHandlerImpl::execute(const std::shared_ptr<Connection> &conn, IPackage *pkg) {
        if (!pkg->isAvailable()) {
            spdlog::warn("{} - Package unavailable", __func__);
            co_return;
        }

        if (pkg->id() >= static_cast<uint32_t>(ProtoType::PROTO_TYPE_MAX)) {
            spdlog::warn("{} - Protocol type out of range", __func__);
            co_return;
        }

        const auto type = static_cast<ProtoType>(pkg->id());

        auto func = find(type);
        if (!func) {
            spdlog::warn("{} - Protocol type not found", __func__);
            co_return;
        }

        const auto pid = std::any_cast<uint64_t>(conn->context());
        // TODO: Find Player Logic

        co_await std::invoke(func, nullptr, dynamic_cast<Package *>(pkg));
    }
} // base