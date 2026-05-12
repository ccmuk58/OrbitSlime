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

    float outerR = 0.5f;
    float innerR = 0.2f;
    XMFLOAT3 p[10];
    for (int i = 0; i < 10; ++i)
    {
        float r;

        if (i % 2 == 0)
        {
            r = outerR;
        }
        else
        {
            r = innerR;
        }

        float angle = XM_PIDIV2 - (i * XM_2PI / 10.0f);
        p[i] = { cosf(angle) * r, sinf(angle) * r, 0.0f };
    }

    std::vector<Vertex> vGold;
    for (int i = 0; i < 10; i++)
    {
        vGold.push_back({ {0, 0, 0}, { 0, 0, 0, 0 } });
        vGold.push_back({ p[i], { 0, 0, 0, 0 } });
        vGold.push_back({ p[(i + 1) % 10], { 0, 0, 0, 0 } });
    }

    D3D11_INPUT_ELEMENT_DESC ied[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    Mesh* gMesh = new Mesh();
    gMesh->vertexCount = 30;
    gMesh->Create(&gEngine.gfx, vGold);

    ShaderSet starShaders = gEngine.gfx.CompileAndCreate(L"Star.hlsl", 0, true, ied, 2);

    ColorMaterial* goldMat = new ColorMaterial(starShaders, { 1, 0.8f, 0, 1 }, gEngine.gfx.Device);
    ColorMaterial* redMat = new ColorMaterial(starShaders, { 1, 0, 0, 1 }, gEngine.gfx.Device);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);

    for (int i = 0; i < 20; i++)
    {
        GameObject* star = new GameObject(dis(gen), dis(gen), 0);

        star->AddComponent(new MeshRenderer(gMesh, (i % 2 == 0) ? goldMat : redMat));
        star->AddComponent(new PlayerController());

        gEngine.world.push_back(star);
    }

    gEngine.Run();

    if (goldMat) { delete goldMat; goldMat = nullptr; }
    if (redMat) { delete redMat; redMat = nullptr; }

    starShaders.Release();

    if (gMesh) { delete gMesh; gMesh = nullptr; }

    return 0;
}
