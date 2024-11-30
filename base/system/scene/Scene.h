#pragma once

#include <cstdint>


class UScene final {

    uint32_t mSceneID;

public:
    UScene();
    ~UScene();

    [[nodiscard]] uint32_t GetSceneID() const;
};

