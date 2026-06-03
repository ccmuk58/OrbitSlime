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
    float dashTimer = 0.15f;    // 대쉬가 유지되는 시간
    float dashCooldown = 1.0f; // 대쉬를 다시 쓰기 위한 대기 시간

public:
    PlayerController(GameObject* target = nullptr, float radius = 0.5f, float speed = 2.5f);
    ~PlayerController() override;


    void Start(GraphicsContext* gfx) override;
    void Input() override;
    void Update(float dt) override;
    void Render(GraphicsContext* gfx) override;
};
