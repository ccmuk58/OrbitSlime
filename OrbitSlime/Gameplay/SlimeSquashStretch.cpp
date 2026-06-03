#include "SlimeSquashStretch.h"

#include "PlayerController.h"

#include <cmath>

SlimeSquashStretch::SlimeSquashStretch(PlayerController* playerController)
    : controller(playerController)
{
}

SlimeSquashStretch::~SlimeSquashStretch()
{
}

void SlimeSquashStretch::Start(GraphicsContext* gfx)
{
    if (pOwner)
    {
        pOwner->scale = idleScale;
    }
}

void SlimeSquashStretch::Input()
{
}

void SlimeSquashStretch::Update(float dt)
{
    if (!pOwner || !controller)
    {
        return;
    }

    const bool isMoving = controller->IsMoving();
    const bool isDashing = controller->IsDashing();

    // When fast movement ends, the slime briefly compresses into the planet.
    if ((wasDashing && !isDashing) || (wasMoving && !isMoving))
    {
        TriggerPlanetSquash();
    }

    if (absorbSquashTimer > 0.0f)
    {
        absorbSquashTimer -= dt;
        SetState(SquashState::AbsorbSquash);
    }
    else if (planetSquashTimer > 0.0f)
    {
        planetSquashTimer -= dt;
        SetState(SquashState::PlanetSquash);
    }
    else if (isDashing)
    {
        SetState(SquashState::Dashing);
    }
    else if (isMoving)
    {
        SetState(SquashState::Moving);
    }
    else
    {
        SetState(SquashState::Idle);
    }

    const XMFLOAT3 targetScale = GetTargetScale();

    // Squash should hit quickly, then relax more softly back to idle.
    const float speed = (state == SquashState::PlanetSquash || state == SquashState::AbsorbSquash)
        ? 14.0f
        : 8.0f;
    const float maxDelta = speed * dt;

    pOwner->scale.x = MoveTowards(pOwner->scale.x, targetScale.x, maxDelta);
    pOwner->scale.y = MoveTowards(pOwner->scale.y, targetScale.y, maxDelta);
    pOwner->scale.z = MoveTowards(pOwner->scale.z, targetScale.z, maxDelta);

    wasMoving = isMoving;
    wasDashing = isDashing;
}

void SlimeSquashStretch::Render(GraphicsContext* gfx)
{
}

XMFLOAT3 SlimeSquashStretch::GetTargetScale() const
{
    switch (state)
    {
    case SquashState::Moving:
        return movingScale;
    case SquashState::Dashing:
        return dashingScale;
    case SquashState::PlanetSquash:
        return planetSquashScale;
    case SquashState::AbsorbSquash:
        return absorbSquashScale;
    case SquashState::Idle:
    default:
        return idleScale;
    }
}

void SlimeSquashStretch::SetState(SquashState nextState)
{
    previousState = state;
    state = nextState;
}

void SlimeSquashStretch::TriggerPlanetSquash()
{
    planetSquashTimer = planetSquashDuration;
}

void SlimeSquashStretch::TriggerAbsorbSquash()
{
    absorbSquashTimer = absorbSquashDuration;
}

float SlimeSquashStretch::MoveTowards(float current, float target, float maxDelta)
{
    const float delta = target - current;
    if (fabsf(delta) <= maxDelta)
    {
        return target;
    }

    return current + (delta > 0.0f ? maxDelta : -maxDelta);
}
