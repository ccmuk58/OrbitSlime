#include "Particle.h"

#include <cmath>
#include <cstdlib>

std::vector<ParticleComponent*> ParticleManager::pool;

void ParticleComponent::Start(GraphicsContext* gfx)
{
}

void ParticleComponent::Input()
{
}

void ParticleComponent::Render(GraphicsContext* gfx)
{
}

void ParticleComponent::Fire(float startX, float startY, float startZ, float dirX, float dirY)
{
    pOwner->pos = { startX, startY, startZ };
    pOwner->isActive = true;

    
    
    float baseAngle = atan2f(dirY, dirX);

    
    
    float spread = (((float)rand() / RAND_MAX) - 0.5f) * 1.5f;
    float angle = baseAngle + spread;
    

    float speed = 0.05f + ((float)rand() / RAND_MAX) * 2.0f;
    vx = cosf(angle) * speed;
    vy = sinf(angle) * speed;
    vz = (((float)rand() / RAND_MAX) - 0.5f) * 2.0f;

    
    rx = (((float)rand() / RAND_MAX) - 0.5f) * 15.0f;
    ry = (((float)rand() / RAND_MAX) - 0.5f) * 15.0f;
    rz = (((float)rand() / RAND_MAX) - 0.5f) * 15.0f;

    lifeTime = 0.2f + ((float)rand() / RAND_MAX) * 0.3f;
    maxLife = lifeTime;
    pOwner->scale = { 0.03f, 0.03f, 0.03f };
}

void ParticleComponent::Update(float dt)
{
    if (!pOwner->isActive) return;

    lifeTime -= dt;
    if (lifeTime <= 0.0f)
    {
        pOwner->isActive = false;
    }
    else
    {
        
        vx *= (1.0f - dt * 6.0f);
        vy *= (1.0f - dt * 6.0f);
        vz *= (1.0f - dt * 6.0f);

        pOwner->pos.x += vx * dt;
        pOwner->pos.y += vy * dt;
        pOwner->pos.z += vz * dt;

        
        pOwner->rot.x += rx * dt;
        pOwner->rot.y += ry * dt;
        pOwner->rot.z += rz * dt;

        
        
        float ratio = lifeTime / maxLife;
        float easeRatio = ratio * ratio;
        pOwner->scale = { 0.03f * easeRatio, 0.03f * easeRatio, 0.03f * easeRatio };
    }
}

void ParticleManager::Spawn(float x, float y, float z, float dirX, float dirY)
{
    int count = 0;
    for (int i = 0; i < (int)pool.size(); i++)
    {
        if (!pool[i]->pOwner->isActive)
        {
            pool[i]->Fire(x, y, z, dirX, dirY);
            count++;
            if (count >= 5) break;
        }
    }
}
