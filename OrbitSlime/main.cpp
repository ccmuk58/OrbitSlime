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
    EngineSettings& settings = EngineSettings::Instance();
    settings.SetWindowTitle(L"Orbit Slime");
    settings.SetWindowClassName(L"Orbit Slime");
    settings.SetWindowSize(800, 800);
    settings.SetResizedWindowSize(1600, 900);
    settings.SetVSync(true);
    settings.SetClearColor(0.1f, 0.2f, 0.3f, 1.0f);

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

    // 행성
    Mesh* planetMesh = new Mesh();
    planetMesh->vertexCount = slimeVertexCnt;
    planetMesh->Create(&gEngine.gfx, slimeVector);
    ShaderSet planetShader = gEngine.gfx.CompileAndCreate(L"Planet.hlsl", 0, true, ied, 2);
    ColorMaterial* planetMat = new ColorMaterial(planetShader, { 0.1f, 0.1f, 0.8f, 1 }, gEngine.gfx.Device);
    GameObject* planet = new GameObject(0, 0, 0);
    planet->scale = { 1.0f, 1.0f, 1.0f };
    planet->AddComponent(new MeshRenderer(planetMesh, planetMat));
    gEngine.world.push_back(planet);

    // 슬라임
    Mesh* slimeMesh = new Mesh();
    slimeMesh->vertexCount = slimeVertexCnt;
    slimeMesh->Create(&gEngine.gfx, slimeVector);
    ShaderSet slimeShader = gEngine.gfx.CompileAndCreate(L"Slime.hlsl", 0, true, ied, 2);
    ColorMaterial* slimeMat = new ColorMaterial(slimeShader, { 0.1f, 0.8f, 0.3f, 1 }, gEngine.gfx.Device);
    GameObject* slime = new GameObject(0, 0.65f, 0);
    slime->scale = { 0.6f, 0.6f, 1.0f };
    slime->AddComponent(new MeshRenderer(slimeMesh, slimeMat));
    // radius 수정 필요(0.4f -> dynamic value)
    slime->AddComponent(new PlayerController(planet, 0.4f, 2.5f));
    gEngine.world.push_back(slime);

    // 소행성
    Mesh* asteroidMesh = new Mesh();
    asteroidMesh->vertexCount = slimeVertexCnt;
    asteroidMesh->Create(&gEngine.gfx, slimeVector);
    ShaderSet asteroidShader = gEngine.gfx.CompileAndCreate(L"Asteroid.hlsl", 0, true, ied, 2);
    ColorMaterial* asteroidMat = new ColorMaterial(asteroidShader, { 0.9f, 0.1f, 0.1f, 1 }, gEngine.gfx.Device);
    GameObject* asteroid = new GameObject(0.5f, 0.5f, 0);
	asteroid->scale = { 0.3f, 0.3f, 1.0f };
    asteroid->AddComponent(new MeshRenderer(asteroidMesh, asteroidMat));
    gEngine.world.push_back(asteroid);

    gEngine.Run();

    if (slimeMat) { delete slimeMat; slimeMat = nullptr; }
	if (asteroidMat) { delete asteroidMat; asteroidMat = nullptr; }

    slimeShader.Release();
	asteroidShader.Release();

    if (slimeMesh) { delete slimeMesh; slimeMesh = nullptr; }
	if (asteroidMesh) { delete asteroidMesh; asteroidMesh = nullptr; }

    return 0;
}
