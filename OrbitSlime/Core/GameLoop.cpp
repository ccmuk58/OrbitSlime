#include "GameLoop.h"
#include "Object.h"
#include "ScoreManager.h" 
#include "AsteroidMovement.h" 
#include "TimerDisplay.h"
#include "Logger.h"

#include <windows.h>

GameLoop::GameLoop()
	: isRunning(true)
{
	world.clear();
	Logger::Log("[Engine] GameLoop Created.");
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

	Logger::Log("[Engine] GameLoop Destroyed. All resources released.");
}

void GameLoop::Initialize(HINSTANCE hInst, LRESULT(CALLBACK* wndProc)(HWND, UINT, WPARAM, LPARAM))
{
	EngineSettings& settings = EngineSettings::Instance();
	win.Initialize(hInst, settings.GetWindowWidth(), settings.GetWindowHeight(), wndProc);
	gfx.InitDX(win.hWnd, settings.GetWindowWidth(), settings.GetWindowHeight());
	camera.Initialize(&gfx);
	globalLightManager.Initialize(&gfx);
	pointLightManager.Initialize(&gfx);
}

void GameLoop::Input()
{
    
    if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
        isRunning = false;

    if (GetAsyncKeyState('F') & 0x0001)
    {
        ToggleFullscreen();
    }

    
    if (currentState == GameState::Title)
    {
        if (GetAsyncKeyState(VK_SPACE) & 0x8000)
        {
            ResetGame();
            currentState = GameState::Playing;
        }
    }

    
    if (currentState == GameState::GameOver)
    {
        
        if (GetAsyncKeyState('R') & 0x8000)
        {
            ResetGame();
            currentState = GameState::Playing;
        }
    }

    
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
    
    if (uiTimer) uiTimer->SetRunning(currentState == GameState::Playing);

   
    
    bool shouldFreeze = (currentState == GameState::Title || currentState == GameState::GameOver);
    for (int i = 0; i < (int)asteroids.size(); i++)
    {
        if (asteroids[i] != nullptr)
        {
            asteroids[i]->isFrozen = shouldFreeze;
        }
    }
    
    
    for (int i = 0; i < (int)world.size(); i++)
    {
        if (world[i] != nullptr)
        {
            
            float applyDt = (currentState == GameState::GameOver) ? 0.0f : dt;
            world[i]->Update(applyDt, &gfx);
        }
    }

    
    if (currentState == GameState::Playing)
    {
        if (ScoreManager::planetHitCount > 4)
        {
            currentState = GameState::GameOver;
        }
    }
}

void GameLoop::Render()
{
    EngineSettings& settings = EngineSettings::Instance();

    
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
	globalLightManager.Bind(gfx.ImmediateContext);

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

	Logger::LogFormat("[Engine] Window Resized to %dx%d", win.Width, win.Height);
}

void GameLoop::ToggleFullscreen()
{
	EngineSettings& settings = EngineSettings::Instance();
	const bool goFullscreen = !settings.IsFullscreen();

	if (goFullscreen)
	{
		windowedWidth = settings.GetResizedWindowWidth();
		windowedHeight = settings.GetResizedWindowHeight();

		HMONITOR monitor = MonitorFromWindow(win.hWnd, MONITOR_DEFAULTTONEAREST);
		MONITORINFO monitorInfo = { sizeof(MONITORINFO) };
		if (GetMonitorInfo(monitor, &monitorInfo))
		{
			const int fullscreenWidth = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
			const int fullscreenHeight = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;

			SetWindowLong(win.hWnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
			SetWindowPos(
				win.hWnd,
				HWND_TOP,
				monitorInfo.rcMonitor.left,
				monitorInfo.rcMonitor.top,
				fullscreenWidth,
				fullscreenHeight,
				SWP_FRAMECHANGED);

			settings.SetWindowSize(fullscreenWidth, fullscreenHeight);
			win.Width = fullscreenWidth;
			win.Height = fullscreenHeight;
		}

		gfx.SetFullscreen(true);
		gfx.Resize(win.Width, win.Height);
		Logger::LogFormat("[Engine] Fullscreen Enabled at %dx%d", win.Width, win.Height);
		return;
	}

	gfx.SetFullscreen(false);
	SetWindowLong(win.hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);

	const int restoreWidth = windowedWidth > 0 ? windowedWidth : settings.GetResizedWindowWidth();
	const int restoreHeight = windowedHeight > 0 ? windowedHeight : settings.GetResizedWindowHeight();
	ResizeWindow(restoreWidth, restoreHeight);

	Logger::LogFormat("[Engine] Fullscreen Disabled. Restored to %dx%d", restoreWidth, restoreHeight);
}

void GameLoop::ResetGame()
{
    ScoreManager::slimeHitCount = 0;
    ScoreManager::planetHitCount = 0;
    
    if (uiTimer) uiTimer->Reset();

    for (int i = 0; i < (int)asteroids.size(); i++)
    {
        if (asteroids[i] != nullptr)
        {
            asteroids[i]->Respawn();
        }
    }
}





