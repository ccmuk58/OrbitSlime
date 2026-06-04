#include "TrailRenderer.h"

TrailRenderer::TrailRenderer(Mesh* mesh, ColorMaterial* material, int maxTrailCount, TrailType type)
    : pMeshData(mesh), pTrailMaterial(material), maxTrailCount(maxTrailCount), trailType(type)
{
}

TrailRenderer::~TrailRenderer()
{
    if (cBuffer)
    {
        cBuffer->Release();
        cBuffer = nullptr;
    }
}

void TrailRenderer::Start(GraphicsContext* gfx)
{
    
    D3D11_BUFFER_DESC cbd = { 0 };
    cbd.Usage = D3D11_USAGE_DEFAULT;
    cbd.ByteWidth = sizeof(ConstantBuffer);
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    gfx->Device->CreateBuffer(&cbd, nullptr, &cBuffer);
}

void TrailRenderer::Input()
{
}

void TrailRenderer::Update(float dt)
{
    if (!pOwner) return;

    recordTimer += dt;
    
    if (recordTimer < recordInterval) return;
    recordTimer = 0.0f;

    if (isEmitting)
    {
        TrailSample sample;
        sample.position = pOwner->pos;
        sample.scale = pOwner->scale;
        sample.rotationZ = pOwner->rot.z;

        samples.insert(samples.begin(), sample);

        if ((int)samples.size() > maxTrailCount)
        {
            samples.pop_back();
        }
    }
    else
    {
        
        
        if (!samples.empty())
        {
            samples.pop_back();
        }
    }
}

void TrailRenderer::Render(GraphicsContext* gfx)
{
    if (!pMeshData || !pTrailMaterial || !cBuffer) return;

    
    XMFLOAT4 originalColor = pTrailMaterial->color;

    
    gfx->SetAlphaBlend(true);
    pTrailMaterial->Bind(gfx->ImmediateContext);

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    gfx->ImmediateContext->IASetVertexBuffers(0, 1, &pMeshData->vBuffer, &stride, &offset);

    if (pMeshData->iBuffer)
    {
        gfx->ImmediateContext->IASetIndexBuffer(pMeshData->iBuffer, DXGI_FORMAT_R32_UINT, 0);
    }

    
    for (int i = (int)samples.size() - 1; i >= 0; i--)
    {
        float t = ((float)i + 1.0f) / (float)maxTrailCount;
        
        float alpha = 0.04f + (1.0f - t) * 0.18f;
        float scale = 0.55f + (1.0f - t) * 0.35f;

        pTrailMaterial->SetColor({ originalColor.x, originalColor.y, originalColor.z, alpha });
        pTrailMaterial->Bind(gfx->ImmediateContext);

        
        const TrailSample& sample = samples[i];
        XMFLOAT3 renderScale = pOwner->scale;
        float renderRotationZ = pOwner->rot.z;

        if (trailType == TrailType::Slime)
        {
            renderScale = sample.scale;
            renderRotationZ = sample.rotationZ;
        }

        float s = 1.0f / (sample.position.z + 1.0f);
        XMMATRIX world = XMMatrixScaling(s * renderScale.x * scale, s * renderScale.y * scale, 1.0f) *
            XMMatrixRotationZ(renderRotationZ) *
            XMMatrixTranslation(sample.position.x, sample.position.y, 0.0f);

        ConstantBuffer cb;
        cb.matWorld = XMMatrixTranspose(world);
        gfx->ImmediateContext->UpdateSubresource(cBuffer, 0, nullptr, &cb, 0, 0);
        gfx->ImmediateContext->VSSetConstantBuffers(0, 1, &cBuffer);

        if (pMeshData->iBuffer)
        {
            gfx->ImmediateContext->DrawIndexed(pMeshData->indexCount, 0, 0);
        }
        else
        {
            gfx->ImmediateContext->Draw(pMeshData->vertexCount, 0);
        }
    }

    
    pTrailMaterial->SetColor(originalColor);
    gfx->SetAlphaBlend(false);
}
