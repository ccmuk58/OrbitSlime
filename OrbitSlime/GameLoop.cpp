#include "GameLoop.h"
#include "Object.h"

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
}

void GameLoop::Input()
{
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

    int objectCount = (int)world.size();
    for (int i = 0; i < objectCount; i++)
    {
        if (world[i] != nullptr)
        {
            world[i]->Input();
        }
    }
}


void GameLoop::Update()
{
    float dt = timer.GetDelta();
    frameCounter.Update(dt);

    for (int i = 0; i < (int)world.size(); i++)
    {
        if (world[i] != nullptr)
        {
            world[i]->Update(dt, &gfx);
        }
    }
}

void GameLoop::Render()
{
    EngineSettings& settings = EngineSettings::Instance();
    gfx.ImmediateContext->ClearRenderTargetView(gfx.RTV, settings.GetClearColor());

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
