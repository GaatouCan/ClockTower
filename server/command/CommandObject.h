#pragma once

#include <string>
#include <vector>


class UCommandObject final {

    std::string mCmdType;
    std::string mCmdOriginParam;
    std::vector<std::string> mCmdParams;
    size_t mIndex;

public:
    UCommandObject() = delete;
    explicit UCommandObject(std::string cmdType, std::string cmdOriginParam = "");

    [[nodiscard]] std::string GetCommandType() const;
    [[nodiscard]] std::string GetCommandOriginParam() const;

    void Reset();

    int ReadInt();
    unsigned int ReadUInt();
    std::string ReadString();
};


