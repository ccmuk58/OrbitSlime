#include "PointLightManager.h"

PointLightManager::PointLightManager()
{
}

PointLightManager::~PointLightManager()
{
    if (pPointLightBuffer)
    {
        pPointLightBuffer->Release();
        pPointLightBuffer = nullptr;
    }
}

void PointLightManager::Initialize(GraphicsContext* gfx)
{
    D3D11_BUFFER_DESC bd = { 0 };
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(PointLightBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    gfx->Device->CreateBuffer(&bd, nullptr, &pPointLightBuffer);
}

void PointLightManager::Clear()
{
    lightCount = 0;
}

void PointLightManager::AddLight(XMFLOAT3 position, float radius, XMFLOAT3 color, float intensity)
{
    if (lightCount >= MAX_POINT_LIGHTS)
    {
        return;
    }

    lights[lightCount].position = position;
    lights[lightCount].radius = radius;
    lights[lightCount].color = color;
    lights[lightCount].intensity = intensity;
    lightCount++;
}

void PointLightManager::Bind(ID3D11DeviceContext* context)
{
    if (!pPointLightBuffer)
    {
        return;
    }

    PointLightBuffer buffer = {};
    buffer.lightCount = lightCount;

    for (int i = 0; i < lightCount; i++)
    {
        buffer.lights[i] = lights[i];
    }

    context->UpdateSubresource(pPointLightBuffer, 0, nullptr, &buffer, 0, 0);
    context->PSSetConstantBuffers(4, 1, &pPointLightBuffer);
}
