#pragma once

#include "Object.h"

#include <vector>

// 1. 파편(입자) 하나하나의 움직임을 제어하는 부품
class ParticleComponent : public Component
{
public:
    float lifeTime = 0.0f;
    float maxLife = 0.0f;
    float vx = 0.0f, vy = 0.0f, vz = 0.0f;

    // ★ 추가: 파편들이 제멋대로 회전하도록 속도 변수 추가!
    float rx = 0.0f, ry = 0.0f, rz = 0.0f;

    void Start(GraphicsContext* gfx) override;
    void Input() override;
    void Render(GraphicsContext* gfx) override;
    void Update(float dt) override;
    void Fire(float startX, float startY, float startZ, float dirX, float dirY);
};

// 2. 파티클들을 한 번에 관리하고 터뜨려주는 매니저
class ParticleManager
{
public:
    static std::vector<ParticleComponent*> pool;

    // ★ 여기도 dirX, dirY 추가!
    static void Spawn(float x, float y, float z, float dirX, float dirY);
};
