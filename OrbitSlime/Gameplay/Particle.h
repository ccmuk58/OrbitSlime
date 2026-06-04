#pragma once

#include "Object.h"

#include <vector>


class ParticleComponent : public Component
{
public:
    float lifeTime = 0.0f;
    float maxLife = 0.0f;
    float vx = 0.0f, vy = 0.0f, vz = 0.0f;

    
    float rx = 0.0f, ry = 0.0f, rz = 0.0f;

    void Start(GraphicsContext* gfx) override;
    void Input() override;
    void Render(GraphicsContext* gfx) override;
    void Update(float dt) override;
    void Fire(float startX, float startY, float startZ, float dirX, float dirY);
};


class ParticleManager
{
public:
    static std::vector<ParticleComponent*> pool;

    
    static void Spawn(float x, float y, float z, float dirX, float dirY);
};
