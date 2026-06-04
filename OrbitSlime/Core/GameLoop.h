#pragma once

#include "Camera.h"
#include "Core.h"
#include "DeltaTime.h"
#include "Frame.h"
#include "GlobalLightManager.h"
#include "PointLightManager.h"

#include <d3d11.h>
#include <vector>

class GameObject;
class AsteroidMovement;
class TimerDisplay;

enum class GameState
{
    Title,    // 시작 화면 (스페이스바 대기)
    Playing,  // 게임 진행 중
    GameOver  // 게임 오버
};

class GameLoop
{
public:
    WindowContext win;
    GraphicsContext gfx;
    DeltaTime timer;
    Frame frameCounter;
    Camera camera;
    GlobalLightManager globalLightManager;
    PointLightManager pointLightManager;
    std::vector<GameObject*> world;
    std::vector<GameObject*> pointLightObjects;
    std::vector<AsteroidMovement*> asteroids;
    bool isRunning = true;
    GameObject* uiTitle = nullptr;    // 타이틀 UI 리모컨
    GameObject* uiGameOver = nullptr; // 게임오버 UI 리모컨

    TimerDisplay* uiTimer = nullptr;

    ID3D11VertexShader* pDefaultVS = nullptr;
    ID3D11PixelShader* pDefaultPS = nullptr;
    ID3D11InputLayout* pDefaultLayout = nullptr;
    int windowedWidth = 0;
    int windowedHeight = 0;

    GameLoop();
    ~GameLoop();

    void Initialize(HINSTANCE hInst, LRESULT(CALLBACK* wndProc)(HWND, UINT, WPARAM, LPARAM));
    void Input();
    void Update();
    void Render();
    void Run();
    void ResizeWindow(int width, int height);

    GameState currentState = GameState::Title;
    void ResetGame();
    void ToggleFullscreen();
};



