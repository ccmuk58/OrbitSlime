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
#include "MeshGenerator.h"
#include "AsteroidMovement.h"
#include "CircleCollider.h"
#include "ObjectShake.h"
#include <d3dcompiler.h>
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
    // 윈도우 세팅
    EngineSettings& settings = EngineSettings::Instance();
    settings.SetWindowTitle(L"Orbit Slime");
    settings.SetWindowClassName(L"Orbit Slime");
    settings.SetWindowSize(800, 800);
    settings.SetResizedWindowSize(1600, 900);
    settings.SetVSync(true);
    settings.SetClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	// 게임루프 생성 및 초기화
    GameLoop gEngine;
    gEngine.Initialize(hI, GlobalWndProc);

	// 셰이더 입력 레이아웃 정의
	const UINT iedCount = 3;
    D3D11_INPUT_ELEMENT_DESC ied[iedCount] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };


	// 메쉬 데이터 생성
	float planetRadius = 0.3f;
	float slimeRadius = 0.1f;
    MeshData planetMeshData = MeshGenerator::CreateSphere(planetRadius, 20, 20);
    MeshData slimeMeshData = MeshGenerator::CreateHemiSphere(slimeRadius, 20, 20);
    MeshData asteroidMeshData = MeshGenerator::CreateSphere(0.05f, 20, 20);


	// 행성
    Mesh* planetMesh = new Mesh();
    planetMesh->Create(&gEngine.gfx, planetMeshData.vertices, planetMeshData.indices);
    ShaderSet planetShader = gEngine.gfx.CompileAndCreate(L"Planet.hlsl", 0, true, ied, iedCount);
    ColorMaterial* planetMat = new ColorMaterial(planetShader, { 0.1f, 0.1f, 0.8f, 1 }, gEngine.gfx.Device);
	planetMat->SetSpecular(0.5f, 32.0f);
    GameObject* planet = new GameObject(0, 0, 0);
    planet->AddComponent(new MeshRenderer(planetMesh, planetMat));
    gEngine.world.push_back(planet);

    ObjectShake* planetShake = new ObjectShake();
    planet->AddComponent(planetShake);

    // 슬라임
    Mesh* slimeMesh = new Mesh();
    slimeMesh->Create(&gEngine.gfx, slimeMeshData.vertices, slimeMeshData.indices);
    ShaderSet slimeShader = gEngine.gfx.CompileAndCreate(L"Slime.hlsl", 0, true, ied, iedCount);
    ColorMaterial* slimeMat = new ColorMaterial(slimeShader, { 0.1f, 0.8f, 0.3f, 1 }, gEngine.gfx.Device);
    slimeMat->SetSpecular(0.9f, 48.0f);
    GameObject* slime = new GameObject(0, 0, 0);
    slime->AddComponent(new MeshRenderer(slimeMesh, slimeMat));
    slime->AddComponent(new PlayerController(planet, planetRadius, 2.5f));
    gEngine.world.push_back(slime);


    // 소행성
    Mesh* asteroidMesh = new Mesh();
    asteroidMesh->Create(&gEngine.gfx, asteroidMeshData.vertices, asteroidMeshData.indices);
    ShaderSet asteroidShader = gEngine.gfx.CompileAndCreate(L"Asteroid.hlsl", 0, true, ied, iedCount);
    ColorMaterial* asteroidMat = new ColorMaterial(asteroidShader, { 0.1f, 0.1f, 0.1f, 1 }, gEngine.gfx.Device);
	asteroidMat->SetSpecular(0.3f, 16.0f);
    GameObject* asteroid = new GameObject(0.5f, 0.5f, 0);
    asteroid->AddComponent(new MeshRenderer(asteroidMesh, asteroidMat));

    // 소행성 10개 소환
    const int ASTEROID_COUNT = 10;
    for (int i = 0; i < ASTEROID_COUNT; i++)
    {
        // 위치, 크기 모두 알아서 할 테니 일단 대충 생성!
        GameObject* asteroid = new GameObject(0, 0, 0);

        asteroid->AddComponent(new MeshRenderer(asteroidMesh, asteroidMat));

        // 속도도 지가 알아서 정할 거니까 빈칸으로 넣기
        AsteroidMovement* astMove = new AsteroidMovement(planet, 0.0f, planetShake);
        asteroid->AddComponent(astMove);

        asteroid->AddComponent(new CircleCollider(slime, 0.05f, planetRadius, astMove));

        gEngine.world.push_back(asteroid);
    }

	// 게임 루프 시작
    gEngine.Run();


    if (slimeMat) { delete slimeMat; slimeMat = nullptr; }
	if (asteroidMat) { delete asteroidMat; asteroidMat = nullptr; }
	if (planetMat) { delete planetMat; planetMat = nullptr; }

    slimeShader.Release();
	asteroidShader.Release();
	planetShader.Release();

    if (slimeMesh) { delete slimeMesh; slimeMesh = nullptr; }
	if (asteroidMesh) { delete asteroidMesh; asteroidMesh = nullptr; }
	if (planetMesh) { delete planetMesh; planetMesh = nullptr; }

    return 0;
}
