#pragma once

#include "Core.h"

#include <directxmath.h>

using namespace DirectX;

struct CameraBuffer
{
    XMMATRIX matView;
    XMMATRIX matProjection;
    XMFLOAT3 cameraPos;
    float padding;
};

class Camera
{
    ID3D11Buffer* pCameraBuffer = nullptr;

    XMFLOAT3 position = { 0.0f, 0.0f, -3.0f };
    XMFLOAT3 target = { 0.0f, 0.0f, 0.0f };
    XMFLOAT3 up = { 0.0f, 1.0f, 0.0f };

    float fovY = XM_PIDIV4;
    float nearZ = 0.1f;
    float farZ = 100.0f;

public:
    Camera();
    ~Camera();

    void Initialize(GraphicsContext* gfx);
    void Bind(GraphicsContext* gfx, int width, int height);

    void SetPosition(XMFLOAT3 value);
    void SetTarget(XMFLOAT3 value);
};
