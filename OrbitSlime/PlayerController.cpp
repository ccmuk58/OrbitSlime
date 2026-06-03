#include "PlayerController.h"

#include <cmath>
#include <windows.h>

PlayerController::PlayerController(GameObject* target, float radius, float speed)
    : Component(), orbitTarget(target), orbitRadius(radius), angularSpeed(speed)
{
}

PlayerController::~PlayerController()
{
}

bool PlayerController::IsMoving() const
{
    return orbitDir != 0.0f;
}

bool PlayerController::IsDashing() const
{
    return dashTimer > 0.0f;
}

float PlayerController::GetMoveDirection() const
{
    return orbitDir;
}

float PlayerController::GetOrbitAngle() const
{
    return orbitAngle;
}
void PlayerController::Start(GraphicsContext* gfx)
{
    if (!orbitTarget)
    {
        return;
    }

    float dx = pOwner->pos.x - orbitTarget->pos.x;
    float dy = pOwner->pos.y - orbitTarget->pos.y;

    if (dx != 0.0f || dy != 0.0f)
    {
        orbitAngle = atan2f(dy, dx);
    }
    pOwner->pos.x = orbitTarget->pos.x + cosf(orbitAngle) * orbitRadius;
    pOwner->pos.y = orbitTarget->pos.y + sinf(orbitAngle) * orbitRadius;
}

void PlayerController::Input()
{
    orbitDir = 0.0f;

    if (GetAsyncKeyState(VK_LEFT) & 0x8000 || GetAsyncKeyState('A') & 0x8000)  orbitDir += 1.0f;
    if (GetAsyncKeyState(VK_RIGHT) & 0x8000 || GetAsyncKeyState('D') & 0x8000) orbitDir -= 1.0f;

    if (GetAsyncKeyState(VK_LSHIFT) & 0x8000)
    {
        if (dashCooldown <= 0.0f)
        {
            dashTimer = 0.15f;    // 0.15초 동안 눈썹 휘날리게 뜀!
            dashCooldown = 1.0f;  // 1초 뒤에 다시 사용 가능

            printf("[Player] Dash 발동!\n"); // 콘솔로 확인용
        }
    }
}

void PlayerController::Update(float dt)
{
    if (!orbitTarget)
    {
        return;
    }

    // 1. 대쉬 타이머와 쿨다운 깎기
    if (dashTimer > 0.0f) dashTimer -= dt;
    if (dashCooldown > 0.0f) dashCooldown -= dt;

    // 2. 현재 스피드(각속도) 결정! 
    float currentAngularSpeed = angularSpeed;

    // 대쉬 중이면 각속도를 3배로 확 올려버림!
    if (dashTimer > 0.0f)
    {
        currentAngularSpeed = angularSpeed * 3.0f;
    }

    // 3. 기존 angularSpeed 대신 currentAngularSpeed를 곱해서 각도 업데이트!
    orbitAngle += orbitDir * currentAngularSpeed * dt;

    // 대쉬 타이머가 남아있으면 잔상 스위치 ON, 아니면 OFF
    if (trail != nullptr)
    {
        trail->isEmitting = (dashTimer > 0.0f);
    }

    pOwner->pos.x = orbitTarget->pos.x + cosf(orbitAngle) * orbitRadius;
    pOwner->pos.y = orbitTarget->pos.y + sinf(orbitAngle) * orbitRadius;
    pOwner->rot.z = orbitAngle - XM_PIDIV2;
}

void PlayerController::Render(GraphicsContext* gfx)
{
}
