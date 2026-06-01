#include "MeshGenerator.h"

#include <cmath>

static float Noise01(int x, int y)
{
    int n = x * 15731 + y * 789221 + 1376312589;
    n = (n << 13) ^ n;
    int nn = (n * (n * n * 60493 + 19990303) + 1376312589) & 0x7fffffff;
    return (float)nn / 2147483647.0f;
}

static XMFLOAT3 Normalize(const XMFLOAT3& v)
{
    float length = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
    if (length <= 0.00001f)
    {
        return { 0.0f, 1.0f, 0.0f };
    }

    return { v.x / length, v.y / length, v.z / length };
}

MeshData MeshGenerator::CreateSphere(float radius, int sliceCount, int stackCount)
{
    MeshData meshData;
    XMFLOAT4 color = { 0, 0, 0, 0 };

	// theta = 0 ~ 2PI, phi = 0 ~ PI
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

MeshData MeshGenerator::CreateHemiSphere(float radius, int sliceCount, int stackCount)
{
    MeshData meshData;
    XMFLOAT4 color = { 0, 0, 0, 0 };

    for (int stack = 0; stack <= stackCount; ++stack)
    {
        float phi = (XM_PIDIV2) * stack / stackCount;
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
    for (int stack = 0; stack < stackCount; ++stack)
    {
        UINT stackStart = stack * sliceCount;
        UINT nextStackStart = (stack + 1) * sliceCount;
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
    return meshData;
}

MeshData MeshGenerator::CreateIrregularSphere(float radius, int sliceCount, int stackCount, float roughness)
{
    MeshData meshData;
    XMFLOAT4 color = { 0, 0, 0, 0 };

    float topRadius = radius * (1.0f + (Noise01(0, 0) * 2.0f - 1.0f) * roughness * 0.45f);
    meshData.vertices.push_back({ { 0.0f, topRadius, 0.0f }, color, { 0.0f, 1.0f, 0.0f } });

    for (int stack = 1; stack < stackCount; ++stack)
    {
        float phi = XM_PI * stack / stackCount;

        for (int slice = 0; slice < sliceCount; ++slice)
        {
            float theta = XM_2PI * slice / sliceCount;
            float noise = Noise01(slice, stack) * 2.0f - 1.0f;
            float localRadius = radius * (1.0f + noise * roughness);

            float x = localRadius * sinf(phi) * cosf(theta);
            float y = localRadius * cosf(phi);
            float z = localRadius * sinf(phi) * sinf(theta);
            XMFLOAT3 normal = Normalize({ x, y, z });

            meshData.vertices.push_back({ { x, y, z }, color, normal });
        }
    }

    float bottomRadius = radius * (1.0f + (Noise01(0, stackCount) * 2.0f - 1.0f) * roughness * 0.45f);
    meshData.vertices.push_back({ { 0.0f, -bottomRadius, 0.0f }, color, { 0.0f, -1.0f, 0.0f } });

    UINT topIndex = 0;
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
