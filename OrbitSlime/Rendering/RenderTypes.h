#pragma once

#include <directxmath.h>

using namespace DirectX;

struct Vertex
{
    XMFLOAT3 pos;
    XMFLOAT4 col;
    XMFLOAT3 normal;
    XMFLOAT2 uv;
};

struct ConstantBuffer
{
    XMMATRIX matWorld;
};

struct ColorBuffer
{
    XMFLOAT4 tintColor;
    float specularStrength;
    float specularPower;
    XMFLOAT2 padding;
};
