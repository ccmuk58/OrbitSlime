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
}

void PlayerController::Update(float dt)
{
    if (!orbitTarget)
    {
        return;
    }

    orbitAngle += orbitDir * angularSpeed * dt;

    pOwner->pos.x = orbitTarget->pos.x + cosf(orbitAngle) * orbitRadius;
    pOwner->pos.y = orbitTarget->pos.y + sinf(orbitAngle) * orbitRadius;
    pOwner->rot.z = orbitAngle - XM_PIDIV2;
}

void PlayerController::Render(GraphicsContext* gfx)
{
}
