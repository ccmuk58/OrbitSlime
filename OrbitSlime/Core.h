#pragma once

#include <windows.h>
#include <d3d11.h>
#include <chrono>
#include <string>

struct ShaderSet
{
    ID3D11VertexShader* vs = nullptr;
    ID3D11PixelShader* ps = nullptr;
    ID3D11InputLayout* layout = nullptr;

    ShaderSet() = default;
    ShaderSet(ID3D11VertexShader* v, ID3D11PixelShader* p, ID3D11InputLayout* l);

    void Release();
};

class DeltaTime
{
    std::chrono::high_resolution_clock::time_point prevTime;

public:
    DeltaTime();
    float GetDelta();
};

class WindowContext
{
public:
    HWND hWnd;
    int Width;
    int Height;
    LPCWSTR windowName;

    WindowContext(LPCWSTR winName = L"DX11 Component Engine");
    ~WindowContext();

    bool Initialize(HINSTANCE hInst, int w, int h, LRESULT(CALLBACK* wndProc)(HWND, UINT, WPARAM, LPARAM));
};

class GraphicsContext
{
public:
    ID3D11Device* Device = nullptr;
    ID3D11DeviceContext* ImmediateContext = nullptr;
    IDXGISwapChain* SwapChain = nullptr;
    ID3D11RenderTargetView* RTV = nullptr;

    bool IsFullscreen = false;
    int VSync = 1;

    ~GraphicsContext();

    bool InitDX(HWND hWnd, int w, int h);
    bool CreateRTV(int w, int h);
    void Resize(int w, int h);
    void SetFullscreen(bool goFull);
    ID3DBlob* CompileShader(const std::string& src, const std::string& entry, const std::string& profile);
    ShaderSet CompileAndCreate(const void* source, size_t length, bool isFile, D3D11_INPUT_ELEMENT_DESC* ied, UINT iedCount);
};
