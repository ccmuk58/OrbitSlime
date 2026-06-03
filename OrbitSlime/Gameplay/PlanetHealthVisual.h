#pragma once

#include "Object.h"
#include "Render.h"

class PlanetHealthVisual : public Component
{
    ColorMaterial* pMaterial = nullptr;
    XMFLOAT4 healthyColor = { 0.82f, 0.68f, 0.47f, 1.0f };
    XMFLOAT4 dangerColor = { 0.4f, 0.05f, 0.0f, 1.0f };
    int maxHitCount = 5;

public:
    PlanetHealthVisual(ColorMaterial* material, int maxHits);
    ~PlanetHealthVisual() override;

    void Start(GraphicsContext* gfx) override;
    void Input() override;
    void Update(float dt) override;
    void Render(GraphicsContext* gfx) override;
};
