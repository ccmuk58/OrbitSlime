#include "SlimePhysics.h"

#include "PlayerController.h"

#include <cmath>

SlimePhysics::SlimePhysics(PlayerController* playerController)
    : controller(playerController)
{
}

SlimePhysics::~SlimePhysics()
{
}

void SlimePhysics::Start(GraphicsContext* gfx)
{
    if (pOwner)
    {
        pOwner->scale = idleScale;
    }
}

void SlimePhysics::Input()
{
}

void SlimePhysics::Update(float dt)
{
    if (!pOwner || !controller)
    {
        return;
    }

    const bool isMoving = controller->IsMoving();
    const bool isDashing = controller->IsDashing();

    
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

void SlimePhysics::Render(GraphicsContext* gfx)
{
}

XMFLOAT3 SlimePhysics::GetTargetScale() const
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

void SlimePhysics::SetState(SquashState nextState)
{
    previousState = state;
    state = nextState;
}

void SlimePhysics::TriggerPlanetSquash()
{
    planetSquashTimer = planetSquashDuration;
}

void SlimePhysics::TriggerAbsorbSquash()
{
    absorbSquashTimer = absorbSquashDuration;
}

float SlimePhysics::MoveTowards(float current, float target, float maxDelta)
{
    const float delta = target - current;
    if (fabsf(delta) <= maxDelta)
    {
        return target;
    }

    return current + (delta > 0.0f ? maxDelta : -maxDelta);
}

