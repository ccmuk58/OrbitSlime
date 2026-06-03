#include "PlanetHealthVisual.h"
#include "ScoreManager.h"

PlanetHealthVisual::PlanetHealthVisual(ColorMaterial* material, int maxHits)
    : pMaterial(material), maxHitCount(maxHits)
{
}

PlanetHealthVisual::~PlanetHealthVisual()
{
}

void PlanetHealthVisual::Start(GraphicsContext* gfx)
{
}

void PlanetHealthVisual::Input()
{
}

void PlanetHealthVisual::Update(float dt)
{
    if (!pMaterial || maxHitCount <= 0) return;

    float t = (float)ScoreManager::planetHitCount / (float)maxHitCount;
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;

    XMFLOAT4 color;
    color.x = healthyColor.x + (dangerColor.x - healthyColor.x) * t;
    color.y = healthyColor.y + (dangerColor.y - healthyColor.y) * t;
    color.z = healthyColor.z + (dangerColor.z - healthyColor.z) * t;
    color.w = healthyColor.w + (dangerColor.w - healthyColor.w) * t;

    pMaterial->SetColor(color);
}

void PlanetHealthVisual::Render(GraphicsContext* gfx)
{
}
