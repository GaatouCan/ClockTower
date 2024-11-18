#include "CommandObject.h"


#include <utils.h>
#include <utility>
#include <spdlog/spdlog.h>


UCommandObject::UCommandObject(std::string cmdType, std::string cmdOriginParam)
    : mCmdType(std::move(cmdType)),
      mCmdOriginParam(std::move(cmdOriginParam)),
      mCmdParams(SplitString(mCmdOriginParam, '|')),
      mIndex(0) {
}

std::string UCommandObject::GetCommandType() const {
    return mCmdType;
}

std::string UCommandObject::GetCommandOriginParam() const {
    return mCmdOriginParam;
}

void UCommandObject::Reset() {
    mIndex = 0;
}

int UCommandObject::ReadInt() {
    if (mIndex >= mCmdParams.size()) {
        spdlog::error("UCommandObject::ReadInt: index out of range");
        return -1;
    }
    return std::stoi(mCmdParams[mIndex++]);
}

unsigned int UCommandObject::ReadUInt() {
    if (mIndex >= mCmdParams.size()) {
        spdlog::error("UCommandObject::ReadInt: index out of range");
        return 0;
    }
    return std::stoul(mCmdParams[mIndex++]);
}

std::string UCommandObject::ReadString() {
    if (mIndex >= mCmdParams.size()) {
        spdlog::error("UCommandObject::ReadInt: index out of range");
        return {};
    }
    return mCmdParams[mIndex++];
}
