#pragma once

#include "Object.h"

#include <directxmath.h>
#include "TrailRenderer.h"

using namespace DirectX;

class TrailRenderer;

class PlayerController : public Component
{
    GameObject* orbitTarget = nullptr;
    float orbitRadius = 0.0f;
    float orbitAngle = XM_PIDIV2;
    float orbitDir = 0.0f;
    float angularSpeed = 2.5f;
    float dashTimer = 0.15f;    
    float dashCooldown = 1.0f; 

public:
    PlayerController(GameObject* target = nullptr, float radius = 0.5f, float speed = 2.5f);
    ~PlayerController() override;

    TrailRenderer* trail = nullptr; 

    bool IsMoving() const;
    bool IsDashing() const;
    float GetMoveDirection() const;
    float GetOrbitAngle() const;


    void Start(GraphicsContext* gfx) override;
    void Input() override;
    void Update(float dt) override;
    void Render(GraphicsContext* gfx) override;
};

