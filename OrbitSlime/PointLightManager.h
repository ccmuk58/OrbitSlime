#pragma once

#include "Core.h"

#include <directxmath.h>

using namespace DirectX;

constexpr int MAX_POINT_LIGHTS = 16;

struct PointLightData
{
    XMFLOAT3 position;
    float radius;
    XMFLOAT3 color;
    float intensity;
};

struct PointLightBuffer
{
    PointLightData lights[MAX_POINT_LIGHTS];
    int lightCount;
    XMFLOAT3 padding;
};

class PointLightManager
{
    ID3D11Buffer* pPointLightBuffer = nullptr;
    PointLightData lights[MAX_POINT_LIGHTS] = {};
    int lightCount = 0;

public:
    PointLightManager();
    ~PointLightManager();

    void Initialize(GraphicsContext* gfx);
    void Clear();
    void AddLight(XMFLOAT3 position, float radius, XMFLOAT3 color, float intensity);
    void Bind(ID3D11DeviceContext* context);
};
