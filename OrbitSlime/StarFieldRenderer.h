#pragma once

#include "Object.h"
#include "Render.h"

class StarFieldRenderer : public Component
{
    Mesh starMesh;
    Material* pMaterial = nullptr;
    ID3D11Buffer* cBuffer = nullptr;

    int starCount = 0;
    float halfWidth = 0.0f;
    float halfHeight = 0.0f;
    float scrollSpeed = 0.0f;
    float scrollOffset = 0.0f;

public:
    StarFieldRenderer(Material* material, int count, float width, float height, float speed);
    ~StarFieldRenderer() override;

    void Start(GraphicsContext* gfx) override;
    void Input() override;
    void Update(float dt) override;
    void Render(GraphicsContext* gfx) override;
};
