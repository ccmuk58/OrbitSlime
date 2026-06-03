#pragma once

#include "Material.h"
#include "Mesh.h"
#include "Object.h"

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
