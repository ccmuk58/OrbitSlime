#include "ObjectShake.h"
#include <cstdlib>

ObjectShake::ObjectShake()
    : shakeDuration(0.0f), shakeMagnitude(0.0f), originX(0.0f), originY(0.0f)
{
}

ObjectShake::~ObjectShake() {}

void ObjectShake::Start(GraphicsContext* gfx)
{
    // 게임 시작 시점의 원래 내 위치를 기억해둠!
    originX = pOwner->pos.x;
    originY = pOwner->pos.y;
}

void ObjectShake::Input() {}

void ObjectShake::Trigger(float duration, float magnitude)
{
    // 흔들림 지시가 들어오면 시간과 강도를 세팅
    shakeDuration = duration;
    shakeMagnitude = magnitude;
}

void ObjectShake::Update(float dt)
{
    if (!pOwner) return;

    if (shakeDuration > 0.0f)
    {
        shakeDuration -= dt;

        // -1.0 ~ 1.0 사이의 무작위 값 생성
        float randX = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;
        float randY = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;

        // 원래 위치에서 무작위 방향으로 살짝 이동시킴
        pOwner->pos.x = originX + randX * shakeMagnitude;
        pOwner->pos.y = originY + randY * shakeMagnitude;
    }
    else
    {
        // 흔들림이 끝나면 원래 위치로 완벽하게 복귀
        pOwner->pos.x = originX;
        pOwner->pos.y = originY;
    }
}

void ObjectShake::Render(GraphicsContext* gfx) {}