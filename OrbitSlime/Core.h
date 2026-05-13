#pragma once

#include <windows.h>
#include <d3d11.h>
#include <chrono>
#include <string>

class EngineSettings
{
    int windowWidth = 800;
    int windowHeight = 600;
    int resizedWindowWidth = 600;
    int resizedWindowHeight = 600;
    bool fullscreen = false;
    bool vSync = true;
    float clearColor[4] = { 0.1f, 0.2f, 0.3f, 1.0f };
    LPCWSTR windowTitle = L"Orbit Slime";
    LPCWSTR windowClassName = L"Orbit Slime";

    EngineSettings() = default;

public:
    static EngineSettings& Instance();

    EngineSettings(const EngineSettings&) = delete;
    EngineSettings& operator=(const EngineSettings&) = delete;

    int GetWindowWidth() const;
    int GetWindowHeight() const;
    void SetWindowSize(int width, int height);

    int GetResizedWindowWidth() const;
    int GetResizedWindowHeight() const;
    void SetResizedWindowSize(int width, int height);

    bool IsFullscreen() const;
    void SetFullscreen(bool value);
    void ToggleFullscreen();

    bool IsVSyncEnabled() const;
    UINT GetPresentInterval() const;
    void SetVSync(bool value);

    const float* GetClearColor() const;
    void SetClearColor(float r, float g, float b, float a);

    LPCWSTR GetWindowTitle() const;
    void SetWindowTitle(LPCWSTR title);

    LPCWSTR GetWindowClassName() const;
    void SetWindowClassName(LPCWSTR className);
};

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

    WindowContext(LPCWSTR winName = EngineSettings::Instance().GetWindowTitle());
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

    ~GraphicsContext();

    bool InitDX(HWND hWnd, int w, int h);
    bool CreateRTV(int w, int h);
    void Resize(int w, int h);
    void SetFullscreen(bool goFull);
    ID3DBlob* CompileShader(const std::string& src, const std::string& entry, const std::string& profile);
    ShaderSet CompileAndCreate(const void* source, size_t length, bool isFile, D3D11_INPUT_ELEMENT_DESC* ied, UINT iedCount);
};
