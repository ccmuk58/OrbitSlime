#pragma once

#include "Camera.h"
#include "Core.h"
#include "DeltaTime.h"
#include "Frame.h"
#include "LightManager.h"
#include "PointLightManager.h"

#include <d3d11.h>
#include <vector>

class GameObject;

class GameLoop
{
public:
    WindowContext win;
    GraphicsContext gfx;
    DeltaTime timer;
    Frame frameCounter;
    Camera camera;
    LightManager lightManager;
    PointLightManager pointLightManager;
    std::vector<GameObject*> world;
    std::vector<GameObject*> pointLightObjects;
    bool isRunning = true;

    ID3D11VertexShader* pDefaultVS = nullptr;
    ID3D11PixelShader* pDefaultPS = nullptr;
    ID3D11InputLayout* pDefaultLayout = nullptr;

    GameLoop();
    ~GameLoop();

    void Initialize(HINSTANCE hInst, LRESULT(CALLBACK* wndProc)(HWND, UINT, WPARAM, LPARAM));
    void Input();
    void Update();
    void Render();
    void Run();
    void ResizeWindow(int width, int height);
};
