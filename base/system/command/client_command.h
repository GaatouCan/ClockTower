#pragma once

#include "abstract_command.h"


class IClientCommand : public IAbstractCommand {

    uint32_t mSender;

public:
    explicit IClientCommand(UCommandObject param);

    void SetSender(uint32_t sender);
    [[nodiscard]] uint32_t GetSender() const;

};

