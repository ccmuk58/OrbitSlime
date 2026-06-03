#pragma once

#include "Core.h"
#include "RenderTypes.h"

class Material
{
public:
    ShaderSet shaders;

    Material(ShaderSet s);
    virtual ~Material();

    virtual void Bind(ID3D11DeviceContext* context) = 0;
};

class ColorMaterial : public Material
{
public:
    XMFLOAT4 color;
    float specularStrength = 0.55f;
    float specularPower = 32.0f;
    ID3D11Buffer* pColorBuffer = nullptr;
    bool useAlphaBlend = false;
    void SetAlphaBlend(bool enabled);

    ColorMaterial(ShaderSet s, XMFLOAT4 col, ID3D11Device* device);
    ~ColorMaterial() override;

    void SetColor(XMFLOAT4 col);
    void SetSpecular(float strength, float power);
    void Bind(ID3D11DeviceContext* context) override;
    bool UseAlphaBlend() const;
};

class TextureMaterial : public Material
{
public:
    ID3D11ShaderResourceView* pSRV;
    ID3D11SamplerState* pSampler;
    bool useAlphaBlend = true;

    TextureMaterial(ShaderSet s, const wchar_t* filePath, ID3D11Device* device);
    virtual ~TextureMaterial();

    void SetAlphaBlend(bool enabled);
    bool UseAlphaBlend() const;
    virtual void Bind(ID3D11DeviceContext* context) override;
};
