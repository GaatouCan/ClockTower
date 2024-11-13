#pragma once

#include "Actor.h"
#include <memory>

class UCharacter : public UActor, public std::enable_shared_from_this<UCharacter> {
public:
    ~UCharacter() override = default;
};
