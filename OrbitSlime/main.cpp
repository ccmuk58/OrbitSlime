/*
================================================================================
 [Engine Architecture]
 1. WindowContext: Win32 창 생성 및 메시지 루프 관리
 2. GraphicsContext: DX11 디바이스, 스왑체인, 셰이더 컴파일 및 영상 설정 관리
 3. DeltaTime: 고해상도 타이머를 이용한 시간 계산
 4. GameObject & Component: 객체 지향적 기능 확장 구조
 5. GameLoop: 전체 흐름(Input-Update-Render) 제어
================================================================================
*/

#include "GameLoop.h"
#include "Object.h"
#include "PlayerController.h"
#include "Render.h"

#include <d3dcompiler.h>
#include <cmath>
#include <random>
#include <vector>

#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

LRESULT CALLBACK GlobalWndProc(HWND h, UINT m, WPARAM w, LPARAM l)
{
    if (m == WM_DESTROY) PostQuitMessage(0);
    return DefWindowProc(h, m, w, l);
}

int WINAPI WinMain(HINSTANCE hI, HINSTANCE, LPSTR, int nS)
{
    GameLoop gEngine;
    gEngine.Initialize(hI, GlobalWndProc);

    const int slimeVertexCnt = 100;
    XMFLOAT3 p[slimeVertexCnt];
    float slimeR = 0.25f;
    for (int i = 0; i < slimeVertexCnt; ++i)
    {
        float angle = XM_PIDIV2 - (i * XM_2PI / slimeVertexCnt);
        p[i] = { cosf(angle) * slimeR, sinf(angle) * slimeR, 0.0f };
    }
    std::vector<Vertex> slimeVector;
    for (int i = 0; i < slimeVertexCnt; i++)
    {
        slimeVector.push_back({ {0, 0, 0}, { 0, 0, 0, 0 } });
        slimeVector.push_back({ p[i], { 0, 0, 0, 0 } });
        slimeVector.push_back({ p[(i + 1) % slimeVertexCnt], { 0, 0, 0, 0 } });
    }

	// Define the input layout for the vertex shader
    D3D11_INPUT_ELEMENT_DESC ied[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    Mesh* slimeMesh = new Mesh();
    slimeMesh->vertexCount = slimeVertexCnt;
    slimeMesh->Create(&gEngine.gfx, slimeVector);
    ShaderSet slimeShader = gEngine.gfx.CompileAndCreate(L"Slime.hlsl", 0, true, ied, 2);
    ColorMaterial* slimeMat = new ColorMaterial(slimeShader, { 0.1f, 0.8f, 0.3f, 1 }, gEngine.gfx.Device);
    GameObject* slime = new GameObject(0, 0 ,0);
    slime->AddComponent(new MeshRenderer(slimeMesh, slimeMat));
    slime->AddComponent(new PlayerController());
    gEngine.world.push_back(slime);

    gEngine.Run();

    if (slimeMat) { delete slimeMat; slimeMat = nullptr; }

    slimeShader.Release();

    if (slimeMesh) { delete slimeMesh; slimeMesh = nullptr; }

    return 0;
}
