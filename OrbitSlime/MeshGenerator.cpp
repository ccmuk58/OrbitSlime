#include "MeshGenerator.h"
#include <cmath>

std::vector<Vertex> MeshGenerator::CreateCircle(float radius, int segmentCount)
{
    std::vector<Vertex> vertices;
    XMFLOAT4 color = { 0, 0, 0, 0 };

    for (int i = 0; i < segmentCount; ++i)
    {
        float angle1 = XM_PIDIV2 - (i * XM_2PI / segmentCount);
        float angle2 = XM_PIDIV2 - ((i + 1) * XM_2PI / segmentCount);

        XMFLOAT3 center = { 0.0f, 0.0f, 0.0f };
        XMFLOAT3 p1 = { cosf(angle1) * radius, sinf(angle1) * radius, 0.0f };
        XMFLOAT3 p2 = { cosf(angle2) * radius, sinf(angle2) * radius, 0.0f };

        vertices.push_back({ center, color });
        vertices.push_back({ p1, color });
        vertices.push_back({ p2, color });
    }

    return vertices;
}

std::vector<Vertex> MeshGenerator::CreateSphere(float radius, int sliceCount, int stackCount)
{
    std::vector<Vertex> vertices;
    XMFLOAT4 color = { 0, 0, 0, 0 };

    auto spherePoint = [&](float phi, float theta)
        {
            float x = radius * sinf(phi) * cosf(theta);
            float y = radius * cosf(phi);
            float z = radius * sinf(phi) * sinf(theta);
            return XMFLOAT3(x, y, z);
        };

    for (int stack = 0; stack < stackCount; ++stack)
    {
        float phi1 = XM_PI * stack / stackCount;
        float phi2 = XM_PI * (stack + 1) / stackCount;

        for (int slice = 0; slice < sliceCount; ++slice)
        {
            float theta1 = XM_2PI * slice / sliceCount;
            float theta2 = XM_2PI * (slice + 1) / sliceCount;

            XMFLOAT3 p1 = spherePoint(phi1, theta1);
            XMFLOAT3 p2 = spherePoint(phi2, theta1);
            XMFLOAT3 p3 = spherePoint(phi2, theta2);
            XMFLOAT3 p4 = spherePoint(phi1, theta2);

            vertices.push_back({ p1, color });
            vertices.push_back({ p2, color });
            vertices.push_back({ p3, color });

            vertices.push_back({ p1, color });
            vertices.push_back({ p3, color });
            vertices.push_back({ p4, color });
        }
    }

    return vertices;
}
