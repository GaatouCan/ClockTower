#include "AbstractPlayer.h"

#include <spdlog/spdlog.h>

IAbstractPlayer::IAbstractPlayer(AConnectionPointer conn)
    : mConn(std::move(conn)),
      mId(std::any_cast<uint64_t>(mConn->GetContext())) {
}

IAbstractPlayer::~IAbstractPlayer() {
}

void IAbstractPlayer::SetConnection(const AConnectionPointer &conn) {
    if (std::any_cast<uint64_t>(conn->GetContext()) == mId) {
        mConn = conn;
    } else {
        spdlog::warn("{} - New Connection 's Context Not Equal Old Player ID", __FUNCTION__);
    }
}

AConnectionPointer IAbstractPlayer::GetConnection() const {
    return mConn;
}

ATcpSocket &IAbstractPlayer::GetSocket() const {
    return mConn->GetSocket();
}

uint64_t IAbstractPlayer::GetPlayerID() const {
    return mId;
}

IPackage *IAbstractPlayer::BuildPackage() const {
    return mConn->BuildPackage();
}

void IAbstractPlayer::Send(IPackage *pkg) const {
    mConn->Send(pkg);
}

void IAbstractPlayer::OnLogin() {
    mLoginTime = std::chrono::steady_clock::now();
}

void IAbstractPlayer::OnLogout() {
    mLoginTime = std::chrono::steady_clock::now();
}

bool IAbstractPlayer::IsOnline() const {
    constexpr ATimePoint zeroTimePoint{};
    const auto now = std::chrono::steady_clock::now();

    return (mLoginTime > zeroTimePoint && mLoginTime < now)
           && (mLogoutTime > zeroTimePoint && mLoginTime < now)
           && (mLogoutTime <= mLoginTime);
}
