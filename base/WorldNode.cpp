#include "WorldNode.h"

UWorldNode::UWorldNode(ATcpSocket socket, UPackagePool &pool)
    : mSocket(std::move(socket)),
      mPool(pool),
      mNodeIndex(0),
      mWatchdogTimer(mSocket.get_executor()) {
}

UWorldNode::~UWorldNode() {
}
