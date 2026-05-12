#pragma once

#include "Object.h"

#include <directxmath.h>

using namespace DirectX;

class PlayerController : public Component
{
    XMFLOAT2 moveDir;
    float rotDir;
    float zoomDir;

public:
    PlayerController();
    ~PlayerController() override;

    void Start(GraphicsContext* gfx) override;
    void Input() override;
    void Update(float dt) override;
    void Render(GraphicsContext* gfx) override;
};
