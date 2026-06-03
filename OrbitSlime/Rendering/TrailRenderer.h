#pragma once

#include "Object.h"
#include "Render.h"

#include <directxmath.h>
#include <vector>

using namespace DirectX;

enum class TrailType
{
    Asteroid = 0,
    Slime = 1
};

struct TrailSample
{
    XMFLOAT3 position;
    XMFLOAT3 scale;
    float rotationZ;
};

class TrailRenderer : public Component
{
    Mesh* pMeshData = nullptr;
    ColorMaterial* pTrailMaterial = nullptr;
    ID3D11Buffer* cBuffer = nullptr;

    std::vector<TrailSample> samples;
    float recordTimer = 0.0f;
    float recordInterval = 0.04f;
    int maxTrailCount = 15;
    TrailType trailType = TrailType::Asteroid;

public:
    TrailRenderer(Mesh* mesh, ColorMaterial* material, int maxTrailCount = 15, TrailType type = TrailType::Asteroid);
    ~TrailRenderer() override;
    bool isEmitting = true;

    void Start(GraphicsContext* gfx) override;
    void Input() override;
    void Update(float dt) override;
    void Render(GraphicsContext* gfx) override;


};
