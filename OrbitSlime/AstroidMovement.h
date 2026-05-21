#pragma once

#include "Object.h"

class AsteroidMovement : public Component
{
    GameObject* target = nullptr;
    float speed = 0.3f;

public:
    AsteroidMovement(GameObject* target = nullptr, float speed = 0.3f);
    ~AsteroidMovement() override;

    void Start(GraphicsContext* gfx) override;
    void Input() override;
    void Update(float dt) override;
    void Respawn();
    void Render(GraphicsContext* gfx) override;
};
