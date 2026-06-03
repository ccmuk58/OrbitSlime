#include "GameLoop.h"
#include "Object.h"
#include "ScoreManager.h" // 점수 확인을 위해 추가
#include "AsteroidMovement.h" // 소행성 리셋을 위해 추가

#include <cstdio>
#include <windows.h>

GameLoop::GameLoop()
	: isRunning(true)
{
	world.clear();
	printf("[Engine] GameLoop Created.\n");
}

GameLoop::~GameLoop()
{
	for (int i = 0; i < (int)world.size(); i++)
	{
		if (world[i])
		{
			delete world[i];
			world[i] = nullptr;
		}
	}
	world.clear();

	if (pDefaultLayout) pDefaultLayout->Release();
	if (pDefaultVS) pDefaultVS->Release();
	if (pDefaultPS) pDefaultPS->Release();

	printf("[Engine] GameLoop Destroyed. All resources released.\n");
}

void GameLoop::Initialize(HINSTANCE hInst, LRESULT(CALLBACK* wndProc)(HWND, UINT, WPARAM, LPARAM))
{
	EngineSettings& settings = EngineSettings::Instance();
	win.Initialize(hInst, settings.GetWindowWidth(), settings.GetWindowHeight(), wndProc);
	gfx.InitDX(win.hWnd, settings.GetWindowWidth(), settings.GetWindowHeight());
	camera.Initialize(&gfx);
	lightManager.Initialize(&gfx);
	pointLightManager.Initialize(&gfx);
}

void GameLoop::Input()
{
    // 1. 공통 키 입력 (ESC 누르면 종료)
    if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
        isRunning = false;

    if (GetAsyncKeyState('F') & 0x0001)
    {
        EngineSettings::Instance().ToggleFullscreen();
        gfx.SetFullscreen(EngineSettings::Instance().IsFullscreen());
    }

    if (GetAsyncKeyState('C') & 0x0001)
    {
        EngineSettings& settings = EngineSettings::Instance();
        ResizeWindow(settings.GetResizedWindowWidth(), settings.GetResizedWindowHeight());
    }

    // 2. ★ 대기(Title) 상태일 때 입력
    if (currentState == GameState::Title)
    {
        if (GetAsyncKeyState(VK_SPACE) & 0x8000)
        {
            ResetGame();
            currentState = GameState::Playing;
            SetWindowText(win.hWnd, L"Orbit Slime | 조심해! 소행성이 날아온다!");
        }
    }

    // 3. ★ 게임오버(GameOver) 상태일 때 입력 (MessageBox 대체!)
    if (currentState == GameState::GameOver)
    {
        // R 키를 누르면 전체화면 깨짐 없이 즉시 다시 시작!
        if (GetAsyncKeyState('R') & 0x8000)
        {
            ResetGame();
            currentState = GameState::Playing;
            SetWindowText(win.hWnd, L"Orbit Slime | 조심해! 소행성이 날아온다!");
        }
    }

    // 4. 게임오버가 아닐 때만 오브젝트들이 키보드 입력을 받음 (슬라임 조작 등)
    if (currentState != GameState::GameOver)
    {
        int objectCount = (int)world.size();
        for (int i = 0; i < objectCount; i++)
        {
            if (world[i] != nullptr)
            {
                world[i]->Input();
            }
        }
    }
}


void GameLoop::Update()
{
	float dt = timer.GetDelta();
	frameCounter.Update(dt);

    if (uiTitle) uiTitle->isActive = (currentState == GameState::Title);
    if (uiGameOver) uiGameOver->isActive = (currentState == GameState::GameOver);

    // 시작 화면일 때 안내 메시지 띄우기
    if (currentState == GameState::Title)
    {
        SetWindowText(win.hWnd, L"Orbit Slime | 방향키로 움직여보세요! [스페이스바]를 누르면 게임이 시작됩니다.");
    }

    // 오브젝트 업데이트 루프 (시간 마법 적용)
    for (int i = 0; i < (int)world.size(); i++)
    {
        if (world[i] != nullptr)
        {
            float applyDt = dt;

            // 시작 전(Title)에는 소행성(2번 이후 오브젝트)들만 시간을 0으로 얼려둠
            if (currentState == GameState::Title && i >= 2)
            {
                applyDt = 0.0f;
            }

            // 게임오버 상태가 되면 모든 액션 정지!
            if (currentState == GameState::GameOver)
            {
                applyDt = 0.0f;
            }

            world[i]->Update(applyDt, &gfx);
        }
    }

    // ★ 플레이 중 슬라임이 맞으면 즉시 게임오버 상태로 전환! (MessageBox 삭제)
    if (currentState == GameState::Playing)
    {
        if (ScoreManager::planetHitCount > 10)
        {
            currentState = GameState::GameOver;

            // 전체화면 모니터 독점을 깨지 않고 완벽하게 안내 메시지 출력!
            SetWindowText(win.hWnd, L"Orbit Slime | [GAME OVER] 다시 시작하려면 [R] 키를 누르세요! 종료는 [ESC]");
        }
    }
}

void GameLoop::Render()
{
    EngineSettings& settings = EngineSettings::Instance();

    // ★ 게임오버 상태가 되면 화면 배경을 핏빛(어두운 빨간색)으로 바꿔서 연출 극대화!
    if (currentState == GameState::GameOver)
    {
        float gameOverColor[4] = { 0.4f, 0.0f, 0.0f, 1.0f };
        gfx.ImmediateContext->ClearRenderTargetView(gfx.RTV, gameOverColor);
    }
    else
    {
        gfx.ImmediateContext->ClearRenderTargetView(gfx.RTV, settings.GetClearColor());
    }

	D3D11_VIEWPORT vp = { 0, 0, (float)settings.GetWindowWidth(), (float)settings.GetWindowHeight(), 0, 1 };
	gfx.ImmediateContext->RSSetViewports(1, &vp);
	gfx.ImmediateContext->OMSetRenderTargets(1, &gfx.RTV, NULL);

	if (pDefaultLayout)
	{
		gfx.ImmediateContext->IASetInputLayout(pDefaultLayout);
	}
	gfx.ImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	camera.Bind(&gfx, settings.GetWindowWidth(), settings.GetWindowHeight());
	lightManager.Bind(gfx.ImmediateContext);

	pointLightManager.Clear();
	for (int i = 0; i < (int)pointLightObjects.size(); i++)
	{
		GameObject* lightObject = pointLightObjects[i];
		if (lightObject)
		{
			pointLightManager.AddLight(lightObject->pos,0.5f,{ 1.0f, 0.38f, 0.08f }, 5.0f);
		}
	}
	pointLightManager.Bind(gfx.ImmediateContext);

	for (int i = 0; i < (int)world.size(); i++)
	{
		if (world[i] != nullptr)
		{
			world[i]->Render(&gfx);
		}
	}
	gfx.SwapChain->Present(settings.GetPresentInterval(), 0);
}

void GameLoop::Run()
{
	MSG msg = {};
	while (msg.message != WM_QUIT && isRunning)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Input();
			Update();
			Render();
		}
	}
}

void GameLoop::ResizeWindow(int width, int height)
{
	EngineSettings::Instance().SetWindowSize(width, height);

	win.Width = width;
	win.Height = height;

	RECT rc = { 0, 0, win.Width, win.Height };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	SetWindowPos(win.hWnd, NULL, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOMOVE | SWP_NOZORDER);
	gfx.Resize(win.Width, win.Height);

	printf("[Engine] Window Resized to %dx%d\n", win.Width, win.Height);
}

void GameLoop::ToggleFullscreen()
{
	EngineSettings& settings = EngineSettings::Instance();
	const bool goFullscreen = !settings.IsFullscreen();

	if (goFullscreen)
	{
		windowedWidth = settings.GetWindowWidth();
		windowedHeight = settings.GetWindowHeight();

		HMONITOR monitor = MonitorFromWindow(win.hWnd, MONITOR_DEFAULTTONEAREST);
		MONITORINFO monitorInfo = { sizeof(MONITORINFO) };
		if (GetMonitorInfo(monitor, &monitorInfo))
		{
			const int fullscreenWidth = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
			const int fullscreenHeight = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;

			settings.SetWindowSize(fullscreenWidth, fullscreenHeight);
			win.Width = fullscreenWidth;
			win.Height = fullscreenHeight;
			gfx.Resize(fullscreenWidth, fullscreenHeight);
		}

		gfx.SetFullscreen(true);
		printf("[Engine] Fullscreen Enabled at %dx%d\n", win.Width, win.Height);
		return;
	}

	gfx.SetFullscreen(false);

	const int restoreWidth = windowedWidth > 0 ? windowedWidth : settings.GetResizedWindowWidth();
	const int restoreHeight = windowedHeight > 0 ? windowedHeight : settings.GetResizedWindowHeight();
	ResizeWindow(restoreWidth, restoreHeight);

	printf("[Engine] Fullscreen Disabled. Restored to %dx%d\n", restoreWidth, restoreHeight);
}

void GameLoop::ResetGame()
{
    ScoreManager::slimeHitCount = 0;
    ScoreManager::planetHitCount = 0;

    for (int i = 0; i < (int)asteroids.size(); i++)
    {
        if (asteroids[i] != nullptr)
        {
            asteroids[i]->Respawn();
        }
    }
}
