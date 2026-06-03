#pragma once
#include "Object.h"
#include <vector>
#include <cstdlib>
#include <cmath>

// 1. 파편(입자) 하나하나의 움직임을 제어하는 부품
class ParticleComponent : public Component
{
public:
    float lifeTime = 0.0f;
    float maxLife = 0.0f;
    float vx = 0.0f, vy = 0.0f, vz = 0.0f;

    // ★ 추가: 파편들이 제멋대로 회전하도록 속도 변수 추가!
    float rx = 0.0f, ry = 0.0f, rz = 0.0f;

    virtual void Start(GraphicsContext* gfx) override {}
    virtual void Input() override {}
    virtual void Render(GraphicsContext* gfx) override {}

    void Fire(float startX, float startY, float startZ, float dirX, float dirY)
    {
        pOwner->pos = { startX, startY, startZ };
        pOwner->isActive = true;

        // =========================================================
        // ★ 무작위 360도가 아니라, 전달받은 방향(dirX, dirY)을 기준으로 각도 계산!
        float baseAngle = atan2f(dirY, dirX);

        // 정확히 일직선으로만 날아가면 심심하니까, 부채꼴 모양으로 퍼지게 랜덤 흩뿌림(Spread) 추가
        // -0.75 ~ +0.75 라디안 (대략 -45도 ~ +45도) 사이로 파편들이 퍼져나감
        float spread = (((float)rand() / RAND_MAX) - 0.5f) * 1.5f;
        float angle = baseAngle + spread;
        // =========================================================

        float speed = 0.05f + ((float)rand() / RAND_MAX) * 2.0f;
        vx = cosf(angle) * speed;
        vy = sinf(angle) * speed;
        vz = (((float)rand() / RAND_MAX) - 0.5f) * 2.0f;

        // 회전속도 셋팅 (이전 코드 유지)
        rx = (((float)rand() / RAND_MAX) - 0.5f) * 15.0f;
        ry = (((float)rand() / RAND_MAX) - 0.5f) * 15.0f;
        rz = (((float)rand() / RAND_MAX) - 0.5f) * 15.0f;

        lifeTime = 0.2f + ((float)rand() / RAND_MAX) * 0.3f;
        maxLife = lifeTime;
        pOwner->scale = { 0.03f, 0.03f, 0.03f };
    }

    void Update(float dt) override
    {
        if (!pOwner->isActive) return;

        lifeTime -= dt;
        if (lifeTime <= 0.0f)
        {
            pOwner->isActive = false;
        }
        else
        {
            // ★ 3. 마찰력(Friction) 적용! 매 프레임마다 속도가 급격히 줄어듦 (진짜 중요!)
            vx *= (1.0f - dt * 6.0f);
            vy *= (1.0f - dt * 6.0f);
            vz *= (1.0f - dt * 6.0f);

            pOwner->pos.x += vx * dt;
            pOwner->pos.y += vy * dt;
            pOwner->pos.z += vz * dt;

            // ★ 4. 빙글빙글 회전 적용
            pOwner->rot.x += rx * dt;
            pOwner->rot.y += ry * dt;
            pOwner->rot.z += rz * dt;

            // ★ 5. 비선형(곡선) 크기 변화! 
            // 일정하게 줄어드는 게 아니라, 끝물에 갑자기 확 사라지게 만듦 (ratio * ratio)
            float ratio = lifeTime / maxLife;
            float easeRatio = ratio * ratio;
            pOwner->scale = { 0.03f * easeRatio, 0.03f * easeRatio, 0.03f * easeRatio };
        }
    }
};

// 2. 파티클들을 한 번에 관리하고 터뜨려주는 매니저
class ParticleManager
{
public:
    static std::vector<ParticleComponent*> pool;

    // ★ 여기도 dirX, dirY 추가!
    static void Spawn(float x, float y, float z, float dirX, float dirY)
    {
        int count = 0;
        for (int i = 0; i < (int)pool.size(); i++)
        {
            if (!pool[i]->pOwner->isActive)
            {
                pool[i]->Fire(x, y, z, dirX, dirY);
                count++;
                if (count >= 8) break;
            }
        }
    }
};