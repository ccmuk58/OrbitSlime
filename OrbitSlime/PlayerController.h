#pragma once

#include "Object.h"

#include <directxmath.h>

using namespace DirectX;

class PlayerController : public Component
{
    GameObject* orbitTarget = nullptr;
    float orbitRadius = 0.0f;
    float orbitAngle = XM_PIDIV2;
    float orbitDir = 0.0f;
    float angularSpeed = 2.5f;

public:
    PlayerController(GameObject* target = nullptr, float radius = 0.5f, float speed = 2.5f);
    ~PlayerController() override;

    void Start(GraphicsContext* gfx) override;
    void Input() override;
    void Update(float dt) override;
    void Render(GraphicsContext* gfx) override;
};
