#include "Mesh.h"

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
    // 숫자처럼 자주 다시 만드는 메쉬를 위해 기존 버퍼를 먼저 해제한다.
    if (vBuffer)
    {
        vBuffer->Release();
        vBuffer = nullptr;
    }
    if (iBuffer)
    {
        iBuffer->Release();
        iBuffer = nullptr;
        indexCount = 0;
    }

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
