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
    static MeshData CreateSphere(float radius, int sliceCount, int stackCount);
    static MeshData CreateHemiSphere(float radius, int sliceCount, int stackCount);
    static MeshData CreateIrregularSphere(float radius, int sliceCount, int stackCount, float roughness);
};
