#include "MeshGenerator.h"
#include <cmath>


MeshData MeshGenerator::CreateSphere(float radius, int sliceCount, int stackCount)
{
    MeshData meshData;
    XMFLOAT4 color = { 0, 0, 0, 0 };
    XMFLOAT3 normal = { 0, 0, 0 };

	// sphere's top vertex
    meshData.vertices.push_back({ { 0.0f, radius, 0.0f }, color , {0, 1, 0} });

    for (int stack = 1; stack < stackCount; ++stack)
    {
        float phi = XM_PI * stack / stackCount;

        for (int slice = 0; slice < sliceCount; ++slice)
        {
            float theta = XM_2PI * slice / sliceCount;
            float x = radius * sinf(phi) * cosf(theta);
            float y = radius * cosf(phi);
            float z = radius * sinf(phi) * sinf(theta);
            XMFLOAT3 normal = { x / radius, y / radius, z / radius };
            meshData.vertices.push_back({ { x, y, z }, color, normal });
        }
    }

    // sphere's bottom vertex
    meshData.vertices.push_back({ { 0.0f, -radius, 0.0f }, color, {0, -1, 0} });
    UINT topIndex = (UINT)0;
    UINT bottomIndex = (UINT)meshData.vertices.size() - 1;

    for (int slice = 0; slice < sliceCount; ++slice)
    {
        UINT current = 1 + slice;
        UINT next = 1 + ((slice + 1) % sliceCount);

        meshData.indices.push_back(topIndex);
        meshData.indices.push_back(current);
        meshData.indices.push_back(next);
    }

    for (int stack = 0; stack < stackCount - 2; ++stack)
    {
        UINT stackStart = 1 + stack * sliceCount;
        UINT nextStackStart = stackStart + sliceCount;

        for (int slice = 0; slice < sliceCount; ++slice)
        {
            UINT current = stackStart + slice;
            UINT next = stackStart + ((slice + 1) % sliceCount);
            UINT below = nextStackStart + slice;
            UINT belowNext = nextStackStart + ((slice + 1) % sliceCount);

            meshData.indices.push_back(current);
            meshData.indices.push_back(below);
            meshData.indices.push_back(belowNext);

            meshData.indices.push_back(current);
            meshData.indices.push_back(belowNext);
            meshData.indices.push_back(next);
        }
    }

    UINT lastStackStart = 1 + (stackCount - 2) * sliceCount;
    for (int slice = 0; slice < sliceCount; ++slice)
    {
        UINT current = lastStackStart + slice;
        UINT next = lastStackStart + ((slice + 1) % sliceCount);
        meshData.indices.push_back(current);
        meshData.indices.push_back(bottomIndex);
        meshData.indices.push_back(next);
    }

    return meshData;
}
