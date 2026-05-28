#pragma once
#include "Object.h"
#include "ObjectShake.h"

class AsteroidMovement : public Component
{
    GameObject* target = nullptr;
    float speed = 0.3f;
    ObjectShake* targetShake;

public:
    AsteroidMovement(GameObject* target = nullptr, float speed = 0.3f, ObjectShake* targetShake = nullptr);
    ~AsteroidMovement() override;

    void Start(GraphicsContext* gfx) override;
    void Input() override;
    void Update(float dt) override;
    void Respawn();
    void Render(GraphicsContext* gfx) override;
};
