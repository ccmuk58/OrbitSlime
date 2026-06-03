#pragma once

#include "Core.h"

#include <directxmath.h>

using namespace DirectX;

// 조명 버퍼 구조체
struct LightBuffer
{
    XMFLOAT3 lightDir;
    float ambient;
    XMFLOAT3 lightColor;
    float diffuseStrength;
};

class LightManager
{
    ID3D11Buffer* pLightBuffer = nullptr;

    XMFLOAT3 lightDir = { -0.5f, 0.5f, 0.0f };
    float ambient = 0.4f;
    XMFLOAT3 lightColor = { 1.0f, 0.95f, 0.85f };
    float diffuseStrength = 0.7f;

public:
    LightManager();
    ~LightManager();

    void Initialize(GraphicsContext* gfx);
    void Bind(ID3D11DeviceContext* context);

    void SetDirectionalLight(XMFLOAT3 dir, XMFLOAT3 color);
    void SetIntensity(float ambientValue, float diffuseValue);
};
