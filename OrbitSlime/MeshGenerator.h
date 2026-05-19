#pragma once

#include "Render.h"

#include <vector>

class MeshGenerator
{
public:
    static std::vector<Vertex> CreateCircle(float radius, int segmentCount);
    static std::vector<Vertex> CreateSphere(float radius, int sliceCount, int stackCount);
};

