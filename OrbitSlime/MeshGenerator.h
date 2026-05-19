#pragma once

#include "Render.h"

#include <vector>

struct MeshData
{
    std::vector<Vertex> vertices;
    std::vector<UINT> indices;
};

class MeshGenerator
{
public:
    static std::vector<Vertex> CreateCircle(float radius, int segmentCount);
    static MeshData CreateSphere(float radius, int sliceCount, int stackCount);
};

