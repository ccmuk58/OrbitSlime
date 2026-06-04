#include "CircleCollider.h"
#include "ScoreManager.h"
#include "SlimePhysics.h"
#include <cmath>

CircleCollider::CircleCollider(GameObject* target, float myBaseRadius, float targetBaseRadius, AsteroidMovement* movement, SlimePhysics* targetSlimePhysics)
    : target(target), myBaseRadius(myBaseRadius), targetBaseRadius(targetBaseRadius), movement(movement), targetSlimePhysics(targetSlimePhysics)
{
}

CircleCollider::~CircleCollider()
{
}

void CircleCollider::Start(GraphicsContext* gfx)
{
}

void CircleCollider::Input()
{
}

void CircleCollider::Update(float dt)
{
    if (!target || !pOwner) return;

    float dx = target->pos.x - pOwner->pos.x;
    float dy = target->pos.y - pOwner->pos.y;
    float distance = sqrtf(dx * dx + dy * dy);

    float myActualRadius = myBaseRadius * pOwner->scale.x;
    float targetActualRadius = targetBaseRadius * target->scale.x;

    if (distance < (myActualRadius + targetActualRadius))
    {
        ScoreManager::slimeHitCount++;
        ScoreManager::PrintScore();
        if (targetSlimePhysics != nullptr)
        {
            targetSlimePhysics->TriggerAbsorbSquash();
        }
        
        if (movement != nullptr)
        {
            movement->Respawn();
        }
    }
}

void CircleCollider::Render(GraphicsContext* gfx)
{
    
}

