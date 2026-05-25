#include "Render.h"

Mesh::Mesh()
{
    vBuffer = nullptr;
    iBuffer = nullptr;
    vertexCount = 0;
    indexCount = 0;
}

Mesh::~Mesh()
{
    if (vBuffer)
    {
        vBuffer->Release();
        vBuffer = nullptr;
    }

    if (iBuffer)
    {
        iBuffer->Release();
        iBuffer = nullptr;
    }
}

void Mesh::Create(GraphicsContext* gfx, const std::vector<Vertex>& vertices)
{
    vertexCount = (UINT)vertices.size();

    D3D11_BUFFER_DESC bd = { 0 };
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(Vertex) * vertexCount;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA sd = { 0 };
    sd.pSysMem = vertices.data();

    gfx->Device->CreateBuffer(&bd, &sd, &vBuffer);
}

void Mesh::Create(GraphicsContext* gfx, const std::vector<Vertex>& vertices, const std::vector<UINT>& indices)
{
    Create(gfx, vertices);

    indexCount = (UINT)indices.size();

    D3D11_BUFFER_DESC bd = { 0 };
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(UINT) * indexCount;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA sd = { 0 };
    sd.pSysMem = indices.data();

    gfx->Device->CreateBuffer(&bd, &sd, &iBuffer);
}

Material::Material(ShaderSet s)
    : shaders(s)
{
}

Material::~Material()
{
}

ColorMaterial::ColorMaterial(ShaderSet s, XMFLOAT4 col, ID3D11Device* device)
    : Material(s), color(col)
{
    D3D11_BUFFER_DESC cbd = { 0 };
    cbd.Usage = D3D11_USAGE_DEFAULT;
    cbd.ByteWidth = sizeof(ColorBuffer);
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    device->CreateBuffer(&cbd, nullptr, &pColorBuffer);
}

ColorMaterial::~ColorMaterial()
{
    if (pColorBuffer) pColorBuffer->Release();
}

void ColorMaterial::SetColor(XMFLOAT4 col)
{
    color = col;
}

void ColorMaterial::SetSpecular(float strength, float power)
{
    specularStrength = strength;
    specularPower = power;
}

void ColorMaterial::Bind(ID3D11DeviceContext* context)
{
    context->IASetInputLayout(shaders.layout);
    context->VSSetShader(shaders.vs, nullptr, 0);
    context->PSSetShader(shaders.ps, nullptr, 0);

    ColorBuffer cb;
    cb.tintColor = color;
    cb.specularStrength = specularStrength;
    cb.specularPower = specularPower;
    cb.padding = { 0.0f, 0.0f };

    context->UpdateSubresource(pColorBuffer, 0, nullptr, &cb, 0, 0);
    context->PSSetConstantBuffers(1, 1, &pColorBuffer);
}

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
}

void MeshRenderer::Input()
{
}

void MeshRenderer::Update(float dt)
{
}
