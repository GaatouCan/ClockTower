#pragma once

class IPlayerComponent {

    class ComponentModule *module_;

public:
    IPlayerComponent() = delete;

    explicit IPlayerComponent(ComponentModule *module);
    virtual ~IPlayerComponent();

    [[nodiscard]] ComponentModule *GetModule() const;
    [[nodiscard]] class Player *GetOwner() const;
};
