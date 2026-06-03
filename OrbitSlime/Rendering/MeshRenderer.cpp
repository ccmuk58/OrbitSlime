#include "MeshRenderer.h"

MeshRenderer::MeshRenderer(Mesh* mesh, Material* mat)
    : Component(), pMeshData(mesh), cBuffer(nullptr), pMaterial(mat)
{
}

MeshRenderer::MeshRenderer(Mesh* mesh)
    : Component(), pMeshData(mesh), cBuffer(nullptr), pMaterial(nullptr)
{
}

MeshRenderer::~MeshRenderer()
{
    if (cBuffer)
    {
        cBuffer->Release();
        cBuffer = nullptr;
    }
}

void MeshRenderer::Start(GraphicsContext* gfx)
{
    D3D11_BUFFER_DESC cbd = { 0 };
    cbd.Usage = D3D11_USAGE_DEFAULT;
    cbd.ByteWidth = sizeof(ConstantBuffer);
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    gfx->Device->CreateBuffer(&cbd, nullptr, &cBuffer);
}

void MeshRenderer::Render(GraphicsContext* gfx)
{
    if (!pMeshData || !pMaterial) return;

    ColorMaterial* colorMat = dynamic_cast<ColorMaterial*>(pMaterial);
    TextureMaterial* textureMat = dynamic_cast<TextureMaterial*>(pMaterial);

    if (colorMat)
    {
        gfx->SetAlphaBlend(colorMat->UseAlphaBlend());
    }
    else if (textureMat)
    {
        gfx->SetAlphaBlend(textureMat->UseAlphaBlend());
    }

    pMaterial->Bind(gfx->ImmediateContext);

    float s = 1.0f / (pOwner->pos.z + 1.0f);
    XMMATRIX world = XMMatrixScaling(s * pOwner->scale.x, s * pOwner->scale.y, 1.0f) *
        XMMatrixRotationZ(pOwner->rot.z) *
        XMMatrixTranslation(pOwner->pos.x, pOwner->pos.y, 0.0f);

    ConstantBuffer cb;
    cb.matWorld = XMMatrixTranspose(world);
    gfx->ImmediateContext->UpdateSubresource(cBuffer, 0, nullptr, &cb, 0, 0);
    gfx->ImmediateContext->VSSetConstantBuffers(0, 1, &cBuffer);

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    gfx->ImmediateContext->IASetVertexBuffers(0, 1, &pMeshData->vBuffer, &stride, &offset);

    if (pMeshData->iBuffer)
    {
        gfx->ImmediateContext->IASetIndexBuffer(pMeshData->iBuffer, DXGI_FORMAT_R32_UINT, 0);
        gfx->ImmediateContext->DrawIndexed(pMeshData->indexCount, 0, 0);
    }
    else
    {
        gfx->ImmediateContext->Draw(pMeshData->vertexCount, 0);
    }
    gfx->SetAlphaBlend(false);
}

void MeshRenderer::Input()
{
}

void MeshRenderer::Update(float dt)
{
}
