#pragma once

#include <string>
#include <vector>


class UCommandObject final {

    std::string mCmdType;
    std::string mCmdOriginParam;
    std::vector<std::string> mCmdParams;

public:
    UCommandObject() = delete;
    explicit UCommandObject(std::string  cmdType, std::string  cmdOriginParam = "");

    [[nodiscard]] std::string GetCommandType() const;
    [[nodiscard]] std::string GetCommandOriginParam() const;

};


