#include "AstroidMovement.h"

#include <cmath>
#include <cstdlib>

// 생성자
AsteroidMovement::AsteroidMovement(GameObject* target, float speed)
    : target(target), speed(speed)
{
}

// 소멸자
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
    if (!target || !pOwner) return;

    float dirX = target->pos.x - pOwner->pos.x;
    float dirY = target->pos.y - pOwner->pos.y;
    float dirZ = target->pos.z - pOwner->pos.z;

    float distance = sqrtf(dirX * dirX + dirY * dirY + dirZ * dirZ);

    if (distance > 0.275f)
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
        // 부딪혔을 때 다시 스폰되는 로직! (ucrtbased.dll 에러 유발 코드 제거)

        // 각도: 0 ~ 359도
        float randomAngle = (rand() % 360) * 3.141592f / 180.0f;

        // 거리: 1.2 ~ 2.5 사이
        float spawnDist = 1.2f + ((float)rand() / (float)RAND_MAX) * 1.3f;

        pOwner->pos.x = cosf(randomAngle) * spawnDist;
        pOwner->pos.y = sinf(randomAngle) * spawnDist;
        pOwner->pos.z = 0.0f;

            // 속도: 0.2 ~ 0.6 사이로 리셋
        this->speed = 0.2f + ((float)rand() / (float)RAND_MAX) * 0.4f;
    }
}
void AsteroidMovement::Respawn()
{
    // 1. 위치 무작위
    float randomAngle = (rand() % 360) * 3.141592f / 180.0f;
    float randomDist = 1.0f + ((float)rand() / (float)RAND_MAX) * 1.5f;
    pOwner->pos.x = cosf(randomAngle) * randomDist;
    pOwner->pos.y = sinf(randomAngle) * randomDist;
    pOwner->pos.z = 0.0f;

    // 2. 크기 무작위 (0.5 ~ 0.9)
    float randomScale = 0.5f + ((float)rand() / (float)RAND_MAX) * 0.4f;
    pOwner->scale = { randomScale, randomScale, 1.0f };

    // 3. 속도 무작위 (0.2 ~ 0.5)
    this->speed = 0.2f + ((float)rand() / (float)RAND_MAX) * 0.3f;
}

void AsteroidMovement::Render(GraphicsContext* gfx)
{
}