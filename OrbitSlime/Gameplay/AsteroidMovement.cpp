#include "AsteroidMovement.h"
#include "ScoreManager.h"
#include "Particle.h"
#include <cmath>
#include <cstdlib>


AsteroidMovement::AsteroidMovement(GameObject* target, float speed, ObjectShake* targetShake)
    : target(target), speed(speed), targetShake(targetShake)
{
}


AsteroidMovement::~AsteroidMovement()
{
}

void AsteroidMovement::Start(GraphicsContext* gfx)
{
    Respawn();
}   

void AsteroidMovement::Input()
{
}

void AsteroidMovement::Update(float dt)
{
    if (isFrozen) return;

    if (!target || !pOwner) return;

    float dirX = target->pos.x - pOwner->pos.x;
    float dirY = target->pos.y - pOwner->pos.y;
    float dirZ = target->pos.z - pOwner->pos.z;

    float distance = sqrtf(dirX * dirX + dirY * dirY + dirZ * dirZ);

    
    if (distance > 0.325f)
    {
        dirX /= distance;
        dirY /= distance;
        dirZ /= distance;

        pOwner->pos.x += dirX * speed * dt;
        pOwner->pos.y += dirY * speed * dt;
        pOwner->pos.z += dirZ * speed * dt;
    }
    else
    {
        

        float backX = pOwner->pos.x - target->pos.x;
        float backY = pOwner->pos.y - target->pos.y;
        ParticleManager::Spawn(pOwner->pos.x, pOwner->pos.y, pOwner->pos.z, backX, backY);
        
        ScoreManager::planetHitCount++;
        ScoreManager::PrintScore();

        if (targetShake != nullptr)
        {
            targetShake->Trigger(0.2f, 0.03f);
        }

        
        Respawn();
    }
}
void AsteroidMovement::Respawn()
{
    
    float randomAngle = (rand() % 360) * 3.141592f / 180.0f;
    float randomDist = 3.0f + ((float)rand() / (float)RAND_MAX) * 3.0f;
    pOwner->pos.x = cosf(randomAngle) * randomDist;
    pOwner->pos.y = sinf(randomAngle) * randomDist;
    pOwner->pos.z = 0.0f;

    
    float randomScale = 0.5f + ((float)rand() / (float)RAND_MAX) * 0.4f;
    pOwner->scale = { randomScale, randomScale, 1.0f };

    
    this->speed = 0.2f + ((float)rand() / (float)RAND_MAX) * 0.3f;
}

void AsteroidMovement::Render(GraphicsContext* gfx)
{
}