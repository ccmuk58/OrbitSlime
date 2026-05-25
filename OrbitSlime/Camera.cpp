#include "Camera.h"

Camera::Camera()
{
}

Camera::~Camera()
{
    if (pCameraBuffer)
    {
        pCameraBuffer->Release();
        pCameraBuffer = nullptr;
    }
}

void Camera::Initialize(GraphicsContext* gfx)
{
    D3D11_BUFFER_DESC cbd = { 0 };
    cbd.Usage = D3D11_USAGE_DEFAULT;
    cbd.ByteWidth = sizeof(CameraBuffer);
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    gfx->Device->CreateBuffer(&cbd, nullptr, &pCameraBuffer);
}

void Camera::Bind(GraphicsContext* gfx, int width, int height)
{
    if (!pCameraBuffer)
    {
        return;
    }

    XMVECTOR eye = XMLoadFloat3(&position);
    XMVECTOR focus = XMLoadFloat3(&target);
    XMVECTOR upDir = XMLoadFloat3(&up);

    float aspect = (height != 0) ? ((float)width / (float)height) : 1.0f;

    XMMATRIX view = XMMatrixLookAtLH(eye, focus, upDir);
    XMMATRIX projection = XMMatrixPerspectiveFovLH(fovY, aspect, nearZ, farZ);

    CameraBuffer cb;
    cb.matView = XMMatrixTranspose(view);
    cb.matProjection = XMMatrixTranspose(projection);
    cb.cameraPos = position;
    cb.padding = 0.0f;

    gfx->ImmediateContext->UpdateSubresource(pCameraBuffer, 0, nullptr, &cb, 0, 0);
    gfx->ImmediateContext->VSSetConstantBuffers(3, 1, &pCameraBuffer);
    gfx->ImmediateContext->PSSetConstantBuffers(3, 1, &pCameraBuffer);
}

void Camera::SetPosition(XMFLOAT3 value)
{
    position = value;
}

void Camera::SetTarget(XMFLOAT3 value)
{
    target = value;
}
