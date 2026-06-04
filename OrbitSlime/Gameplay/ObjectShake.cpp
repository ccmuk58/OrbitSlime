#include "ObjectShake.h"
#include <cstdlib>

ObjectShake::ObjectShake()
    : shakeDuration(0.0f), shakeMagnitude(0.0f), originX(0.0f), originY(0.0f)
{
}

ObjectShake::~ObjectShake() {}

void ObjectShake::Start(GraphicsContext* gfx)
{
    
    originX = pOwner->pos.x;
    originY = pOwner->pos.y;
}

void ObjectShake::Input() {}

void ObjectShake::Trigger(float duration, float magnitude)
{
    
    shakeDuration = duration;
    shakeMagnitude = magnitude;
}

void ObjectShake::Update(float dt)
{
    if (!pOwner) return;

    if (shakeDuration > 0.0f)
    {
        shakeDuration -= dt;

        
        float randX = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;
        float randY = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;

        
        pOwner->pos.x = originX + randX * shakeMagnitude;
        pOwner->pos.y = originY + randY * shakeMagnitude;
    }
    else
    {
        
        pOwner->pos.x = originX;
        pOwner->pos.y = originY;
    }
}

void ObjectShake::Render(GraphicsContext* gfx) {}