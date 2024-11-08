#pragma once

#include <memory>

class ICharacter : public std::enable_shared_from_this<ICharacter> {
public:
    virtual ~ICharacter() = default;
};
