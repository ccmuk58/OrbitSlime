#include "StarFieldRenderer.h"

#include <cstdlib>

float RandomRange(float minValue, float maxValue)
{
    float t = (float)rand() / (float)RAND_MAX;
    return minValue + (maxValue - minValue) * t;
}

StarFieldRenderer::StarFieldRenderer(Material* material, int count, float width, float height, float speed)
    : pMaterial(material), starCount(count), halfWidth(width * 0.5f), halfHeight(height * 0.5f), scrollSpeed(speed)
{
}

StarFieldRenderer::~StarFieldRenderer()
{
    if (cBuffer)
    {
        cBuffer->Release();
        cBuffer = nullptr;
    }
}

void StarFieldRenderer::Start(GraphicsContext* gfx)
{
    srand(1);

    std::vector<Vertex> vertices;
    vertices.reserve(starCount * 6);

    for (int i = 0; i < starCount; i++)
    {
        float x = RandomRange(-halfWidth, halfWidth);
        float y = RandomRange(-halfHeight, halfHeight);
        float size = RandomRange(0.0025f, 0.008f);
        float brightness = RandomRange(0.35f, 1.0f);
        float blueTint = RandomRange(0.85f, 1.0f);
        XMFLOAT4 color = { brightness, brightness, brightness * blueTint, 1.0f };
        XMFLOAT3 normal = { 0.0f, 0.0f, -1.0f };

        Vertex v0 = { { x - size, y - size, 0.0f }, color, normal };
        Vertex v1 = { { x - size, y + size, 0.0f }, color, normal };
        Vertex v2 = { { x + size, y + size, 0.0f }, color, normal };
        Vertex v3 = { { x + size, y - size, 0.0f }, color, normal };

        vertices.push_back(v0);
        vertices.push_back(v1);
        vertices.push_back(v2);
        vertices.push_back(v0);
        vertices.push_back(v2);
        vertices.push_back(v3);
    }

    starMesh.Create(gfx, vertices);

    D3D11_BUFFER_DESC cbd = { 0 };
    cbd.Usage = D3D11_USAGE_DEFAULT;
    cbd.ByteWidth = sizeof(ConstantBuffer);
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    gfx->Device->CreateBuffer(&cbd, nullptr, &cBuffer);
}

void StarFieldRenderer::Input()
{
}

void StarFieldRenderer::Update(float dt)
{
    scrollOffset += scrollSpeed * dt;
    if (scrollOffset > halfWidth)
    {
        scrollOffset -= halfWidth * 2.0f;
    }
}

void StarFieldRenderer::Render(GraphicsContext* gfx)
{
    if (!pMaterial || !cBuffer || !starMesh.vBuffer) return;

    pMaterial->Bind(gfx->ImmediateContext);

    XMMATRIX world = XMMatrixTranslation(scrollOffset, 0.0f, 0.0f);

    ConstantBuffer cb;
    cb.matWorld = XMMatrixTranspose(world);
    gfx->ImmediateContext->UpdateSubresource(cBuffer, 0, nullptr, &cb, 0, 0);
    gfx->ImmediateContext->VSSetConstantBuffers(0, 1, &cBuffer);

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    gfx->ImmediateContext->IASetVertexBuffers(0, 1, &starMesh.vBuffer, &stride, &offset);
    gfx->ImmediateContext->Draw(starMesh.vertexCount, 0);
}
