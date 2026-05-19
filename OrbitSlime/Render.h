#pragma once

#include "Core.h"
#include "Object.h"

#include <directxmath.h>
#include <vector>

using namespace DirectX;

struct Vertex
{
    XMFLOAT3 pos;
    XMFLOAT4 col;
	XMFLOAT3 normal;
};

struct ConstantBuffer
{
    XMMATRIX matWorld;
};
struct ColorBuffer
{
    XMFLOAT4 tintColor;
};
struct LightBuffer
{
    XMFLOAT3 lightDir;
    float ambient;
    XMFLOAT3 lightColor;
    float diffuseStrength;
};

struct Mesh
{
public:
    ID3D11Buffer* vBuffer;
    ID3D11Buffer* iBuffer;
    UINT vertexCount;
    UINT indexCount;

    Mesh();
    ~Mesh();

    void Create(GraphicsContext* gfx, const std::vector<Vertex>& vertices);
    void Create(GraphicsContext* gfx, const std::vector<Vertex>& vertices, const std::vector<UINT>& indices);
};

class Material
{
public:
    ShaderSet shaders;

    Material(ShaderSet s);
    virtual ~Material();

    virtual void Bind(ID3D11DeviceContext* context) = 0;
};


class ColorMaterial : public Material
{
public:
    XMFLOAT4 color;
    ID3D11Buffer* pColorBuffer = nullptr;
	ID3D11Buffer* pLightBuffer = nullptr;
    XMFLOAT3 lightDir = { -0.5f, 0.5f, 0.0f };
    float ambient = 0.7f;
	XMFLOAT3 lightColor = { 1.0f, 0.95f, 0.85f };
	float diffuseStrength = 0.7f;

    ColorMaterial(ShaderSet s, XMFLOAT4 col, ID3D11Device* device);
    ~ColorMaterial() override;

    void SetColor(XMFLOAT4 col);
    void Bind(ID3D11DeviceContext* context) override;
};

class MeshRenderer : public Component
{
public:
    Mesh* pMeshData = nullptr;
    ID3D11Buffer* cBuffer = nullptr;
    Material* pMaterial = nullptr;

    MeshRenderer(Mesh* mesh, Material* mat);
    MeshRenderer(Mesh* mesh);
    ~MeshRenderer() override;

    void Start(GraphicsContext* gfx) override;
    void Render(GraphicsContext* gfx) override;
    void Input() override;
    void Update(float dt) override;
};
