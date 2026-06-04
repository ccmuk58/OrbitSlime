#include "GlobalLightManager.h"

GlobalLightManager::GlobalLightManager()
{
}

GlobalLightManager::~GlobalLightManager()
{
    if (pLightBuffer)
    {
        pLightBuffer->Release();
        pLightBuffer = nullptr;
    }
}

void GlobalLightManager::Initialize(GraphicsContext* gfx)
{
    D3D11_BUFFER_DESC lbd = { 0 };
    lbd.Usage = D3D11_USAGE_DEFAULT;
    lbd.ByteWidth = sizeof(LightBuffer);
    lbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    gfx->Device->CreateBuffer(&lbd, nullptr, &pLightBuffer);
}

void GlobalLightManager::Bind(ID3D11DeviceContext* context)
{
    if (!pLightBuffer)
    {
        return;
    }

    LightBuffer lb;
    lb.lightDir = lightDir;
    lb.ambient = ambient;
    lb.lightColor = lightColor;
    lb.diffuseStrength = diffuseStrength;

    context->UpdateSubresource(pLightBuffer, 0, nullptr, &lb, 0, 0);
    context->PSSetConstantBuffers(2, 1, &pLightBuffer);
}

void GlobalLightManager::SetDirectionalLight(XMFLOAT3 dir, XMFLOAT3 color)
{
    lightDir = dir;
    lightColor = color;
}

void GlobalLightManager::SetIntensity(float ambientValue, float diffuseValue)
{
    ambient = ambientValue;
    diffuseStrength = diffuseValue;
}


