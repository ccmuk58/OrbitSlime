#pragma once

#include "Object.h"

#include <directxmath.h>

using namespace DirectX;

class PlayerController;

class SlimeSquashStretch : public Component
{
private:
    enum class SquashState
    {
        Idle,
        Moving,
        Dashing,
        PlanetSquash,
        AbsorbSquash
    };

    PlayerController* controller = nullptr;
    SquashState state = SquashState::Idle;
    SquashState previousState = SquashState::Idle;

    float planetSquashTimer = 0.0f;
    float planetSquashDuration = 0.18f;
    float absorbSquashTimer = 0.0f;
    float absorbSquashDuration = 0.16f;
    bool wasMoving = false;
    bool wasDashing = false;

    XMFLOAT3 idleScale = { 1.0f, 1.0f, 1.0f };
    XMFLOAT3 movingScale = { 1.22f, 0.86f, 1.0f };
    XMFLOAT3 dashingScale = { 1.45f, 0.72f, 1.0f };
    XMFLOAT3 planetSquashScale = { 1.35f, 0.64f, 1.0f };
    XMFLOAT3 absorbSquashScale = { 0.78f, 1.32f, 1.0f };

    XMFLOAT3 GetTargetScale() const;
    void SetState(SquashState nextState);
    void TriggerPlanetSquash();
    static float MoveTowards(float current, float target, float maxDelta);

public:
    SlimeSquashStretch(PlayerController* playerController);
    ~SlimeSquashStretch() override;

    void TriggerAbsorbSquash();

    void Start(GraphicsContext* gfx) override;
    void Input() override;
    void Update(float dt) override;
    void Render(GraphicsContext* gfx) override;
};
