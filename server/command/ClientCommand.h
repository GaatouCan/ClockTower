#pragma once

#include "AbstractCommand.h"


class IClientCommand : public IAbstractCommand {

    uint64_t mSender;

public:
    explicit IClientCommand(UCommandObject param);

    void SetSender(uint64_t sender);
    [[nodiscard]] uint64_t GetSender() const;

};

