#pragma once

#include "Core.h"
#include "RenderTypes.h"

#include <vector>

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
