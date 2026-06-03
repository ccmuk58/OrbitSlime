#pragma once

#include "Object.h"
#include "Render.h"

#include <directxmath.h>
#include <vector>

using namespace DirectX;

class AsteroidTrailRenderer : public Component
{
    Mesh* pMeshData = nullptr;
    ColorMaterial* pTrailMaterial = nullptr;
    ID3D11Buffer* cBuffer = nullptr;

    std::vector<XMFLOAT3> positions;
    float recordTimer = 0.0f;
    float recordInterval = 0.04f;
    int maxTrailCount = 10;

public:
    AsteroidTrailRenderer(Mesh* mesh, ColorMaterial* material);
    ~AsteroidTrailRenderer() override;
    bool isEmitting = true;

    void Start(GraphicsContext* gfx) override;
    void Input() override;
    void Update(float dt) override;
    void Render(GraphicsContext* gfx) override;


};
