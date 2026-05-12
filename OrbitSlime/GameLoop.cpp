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
    win.Initialize(hInst, 800, 600, wndProc);
    gfx.InitDX(win.hWnd, 800, 600);
}

void GameLoop::Input()
{
    if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
        isRunning = false;
    if (GetAsyncKeyState('F') & 0x0001)
        gfx.SetFullscreen(!gfx.IsFullscreen);

    if (GetAsyncKeyState('C') & 0x0001)
    {
        win.Width = 600;
        win.Height = 600;

        RECT rc = { 0, 0, win.Width, win.Height };
        AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

        SetWindowPos(win.hWnd, NULL, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOMOVE | SWP_NOZORDER);
        gfx.Resize(win.Width, win.Height);

        printf("[Engine] Window Resized to 600x600\n");
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
    float col[] = { 0.1f, 0.2f, 0.3f, 1.0f };
    gfx.ImmediateContext->ClearRenderTargetView(gfx.RTV, col);

    D3D11_VIEWPORT vp = { 0, 0, (float)win.Width, (float)win.Height, 0, 1 };
    gfx.ImmediateContext->RSSetViewports(1, &vp);
    gfx.ImmediateContext->OMSetRenderTargets(1, &gfx.RTV, NULL);

    if (pDefaultLayout)
    {
        gfx.ImmediateContext->IASetInputLayout(pDefaultLayout);
    }
    gfx.ImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    for (int i = 0; i < (int)world.size(); i++)
    {
        if (world[i] != nullptr)
        {
            world[i]->Render(&gfx);
        }
    }
    gfx.SwapChain->Present(gfx.VSync, 0);
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
