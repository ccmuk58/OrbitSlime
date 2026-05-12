#include "Render.h"

Mesh::Mesh()
{
    vBuffer = nullptr;
    vertexCount = 0;
}

Mesh::~Mesh()
{
    if (vBuffer)
    {
        vBuffer->Release();
        vBuffer = nullptr;
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

void ColorMaterial::Bind(ID3D11DeviceContext* context)
{
    context->IASetInputLayout(shaders.layout);
    context->VSSetShader(shaders.vs, nullptr, 0);
    context->PSSetShader(shaders.ps, nullptr, 0);

    ColorBuffer cb = { color };
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
    gfx->ImmediateContext->Draw(pMeshData->vertexCount, 0);
}

void MeshRenderer::Input()
{
}

void MeshRenderer::Update(float dt)
{
}
