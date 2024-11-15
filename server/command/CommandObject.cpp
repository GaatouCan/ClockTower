#include "CommandObject.h"
#include "../base/utils.h"

#include <utility>


UCommandObject::UCommandObject(std::string cmdType, std::string cmdOriginParam)
    : mCmdType(std::move(cmdType)),
      mCmdOriginParam(std::move(cmdOriginParam)),
      mCmdParams(SplitString(mCmdOriginParam, '|')) {
}

std::string UCommandObject::GetCommandType() const {
    return mCmdType;
}

std::string UCommandObject::GetCommandOriginParam() const {
    return mCmdOriginParam;
}
