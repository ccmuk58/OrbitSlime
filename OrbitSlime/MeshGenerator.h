#pragma once

#include "Render.h"

#include <vector>

// 메쉬 데이터 구조체
struct MeshData
{
    std::vector<Vertex> vertices;
    std::vector<UINT> indices;
};

class MeshGenerator
{
public:
    // 구 생성 함수
    static MeshData CreateSphere(float radius, int sliceCount, int stackCount);
    static MeshData CreateHemiSphere(float radius, int sliceCount, int stackCount);
};

