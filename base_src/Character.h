#pragma once

#include <memory>

#include "Object.h"

class ICharacter : public std::enable_shared_from_this<ICharacter>, public UObject {
public:
    ~ICharacter() override = default;
};
