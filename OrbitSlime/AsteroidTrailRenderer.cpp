#include "AsteroidTrailRenderer.h"

AsteroidTrailRenderer::AsteroidTrailRenderer(Mesh* mesh, ColorMaterial* material)
    : pMeshData(mesh), pTrailMaterial(material)
{
}

AsteroidTrailRenderer::~AsteroidTrailRenderer()
{
    if (cBuffer)
    {
        cBuffer->Release();
        cBuffer = nullptr;
    }
}

void AsteroidTrailRenderer::Start(GraphicsContext* gfx)
{
    // 잔상은 같은 메시를 여러 위치에 다시 그리므로, 위치마다 다른 world matrix를 넘길 상수버퍼가 필요하다.
    D3D11_BUFFER_DESC cbd = { 0 };
    cbd.Usage = D3D11_USAGE_DEFAULT;
    cbd.ByteWidth = sizeof(ConstantBuffer);
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    gfx->Device->CreateBuffer(&cbd, nullptr, &cBuffer);
}

void AsteroidTrailRenderer::Input()
{
}

void AsteroidTrailRenderer::Update(float dt)
{
    if (!pOwner) return;

    recordTimer += dt;
    // 매 프레임 위치를 저장하면 잔상이 너무 촘촘해지므로 일정 간격마다 기록한다.
    if (isEmitting)
    {
        positions.insert(positions.begin(), pOwner->pos);

        if ((int)positions.size() > maxTrailCount)
        {
            positions.pop_back();
        }
    }
    else
    {
        // 스위치가 꺼지면, 새로운 위치를 추가하지 않고 꼬리(이전 위치들)를 하나씩 지움
        // 이렇게 하면 대쉬가 끝났을 때 꼬리가 제자리에 멈춰있지 않고 자연스럽게 스르륵 사라짐
        if (!positions.empty())
        {
            positions.pop_back();
        }
    }
}

void AsteroidTrailRenderer::Render(GraphicsContext* gfx)
{
    if (!pMeshData || !pTrailMaterial || !cBuffer) return;

    // material 색을 draw마다 바꾸므로, 렌더링 후 원래 색으로 복구한다.
    XMFLOAT4 originalColor = pTrailMaterial->color;

    // 잔상은 투명하게 겹쳐 그리기 때문에 alpha blending을 켠다.
    gfx->SetAlphaBlend(true);
    pTrailMaterial->Bind(gfx->ImmediateContext);

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    gfx->ImmediateContext->IASetVertexBuffers(0, 1, &pMeshData->vBuffer, &stride, &offset);

    if (pMeshData->iBuffer)
    {
        gfx->ImmediateContext->IASetIndexBuffer(pMeshData->iBuffer, DXGI_FORMAT_R32_UINT, 0);
    }

    // 오래된 위치부터 먼저 그려서 가까운 잔상이 더 위에 자연스럽게 겹치게 한다.
    for (int i = (int)positions.size() - 1; i >= 0; i--)
    {
        float t = ((float)i + 1.0f) / (float)maxTrailCount;
        // 현재 위치에 가까운 잔상일수록 더 크고 진하게 보이게 한다.
        float alpha = 0.04f + (1.0f - t) * 0.18f;
        float scale = 0.55f + (1.0f - t) * 0.35f;

        pTrailMaterial->SetColor({ 1.0f, 0.28f, 0.05f, alpha });
        pTrailMaterial->Bind(gfx->ImmediateContext);

        // 저장된 이전 위치마다 world matrix를 새로 만들어 같은 mesh를 다시 그린다.
        float s = 1.0f / (positions[i].z + 1.0f);
        XMMATRIX world = XMMatrixScaling(s * pOwner->scale.x * scale,s * pOwner->scale.y * scale,1.0f) *
            XMMatrixRotationZ(pOwner->rot.z) *
            XMMatrixTranslation(positions[i].x, positions[i].y, 0.0f);

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

    // 다른 오브젝트 렌더링에 영향이 남지 않도록 상태와 material 색을 복구한다.
    pTrailMaterial->SetColor(originalColor);
    gfx->SetAlphaBlend(false);
}
