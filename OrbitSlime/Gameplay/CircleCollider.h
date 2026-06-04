#pragma once
#include "Object.h"
#include "AsteroidMovement.h"

class SlimePhysics;


class CircleCollider : public Component
{
private:
    GameObject* target = nullptr; 
    float myBaseRadius;           
    float targetBaseRadius;       
    AsteroidMovement* movement;
    SlimePhysics* targetSlimePhysics;

public:
    CircleCollider(GameObject* target, float myBaseRadius, float targetBaseRadius, AsteroidMovement* movement, SlimePhysics* targetSlimePhysics = nullptr);
    ~CircleCollider() override;

    void Start(GraphicsContext* gfx) override;
    void Input() override;
    void Update(float dt) override;
    void Render(GraphicsContext* gfx) override;
};

