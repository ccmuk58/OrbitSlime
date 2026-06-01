#include "AsteroidMovement.h"
#include "ScoreManager.h"
#include <cmath>
#include <cstdlib>

// 생성자
AsteroidMovement::AsteroidMovement(GameObject* target, float speed, ObjectShake* targetShake)
    : target(target), speed(speed), targetShake(targetShake)
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

    // 주의: 행성 충돌 크기
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
        //소행성이 행성 중심에 닿아버림

        //행성 피격 횟수 1 증가시키고 점수 출력
        ScoreManager::planetHitCount++;
        ScoreManager::PrintScore();

        if (targetShake != nullptr)
        {
            targetShake->Trigger(0.2f, 0.03f);
        }

        //다시 멀리서 날아오도록 무작위 스폰
        Respawn();
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