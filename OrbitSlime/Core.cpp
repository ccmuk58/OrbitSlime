#include "Core.h"

#include <d3dcompiler.h>

ShaderSet::ShaderSet(ID3D11VertexShader* v, ID3D11PixelShader* p, ID3D11InputLayout* l)
    : vs(v), ps(p), layout(l)
{
}

void ShaderSet::Release()
{
    if (vs) { vs->Release(); vs = nullptr; }
    if (ps) { ps->Release(); ps = nullptr; }
    if (layout) { layout->Release(); layout = nullptr; }
}

WindowContext::WindowContext(LPCWSTR winName)
    : hWnd(nullptr), Width(800), Height(600), windowName(winName)
{
}

WindowContext::~WindowContext()
{
    UnregisterClass(EngineSettings::Instance().GetWindowClassName(), GetModuleHandle(NULL));
}

bool WindowContext::Initialize(HINSTANCE hInst, int w, int h, LRESULT(CALLBACK* wndProc)(HWND, UINT, WPARAM, LPARAM))
{
    Width = w;
    Height = h;

    WNDCLASSEX wc = { sizeof(WNDCLASSEX) };
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = wndProc;
    wc.hInstance = hInst;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = EngineSettings::Instance().GetWindowClassName();

    if (!RegisterClassEx(&wc)) return false;

    RECT rc = { 0, 0, w, h };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

    hWnd = CreateWindow(EngineSettings::Instance().GetWindowClassName(), windowName, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top,
        NULL, NULL, hInst, NULL);

    if (!hWnd) return false;

    ShowWindow(hWnd, SW_SHOW);
    return true;
}

GraphicsContext::~GraphicsContext()
{
    if (RTV)
        RTV->Release();
    if (SwapChain)
        SwapChain->Release();
    if (ImmediateContext)
        ImmediateContext->Release();
    if (Device)
        Device->Release();
}

bool GraphicsContext::InitDX(HWND hWnd, int w, int h)
{
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 1;
    sd.BufferDesc.Width = w;
    sd.BufferDesc.Height = h;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, NULL, 0,
        D3D11_SDK_VERSION, &sd, &SwapChain, &Device, NULL, &ImmediateContext);

    return SUCCEEDED(hr) && CreateRTV(w, h);
}

bool GraphicsContext::CreateRTV(int w, int h)
{
    if (RTV) RTV->Release();
    ID3D11Texture2D* pBB;
    SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBB);
    Device->CreateRenderTargetView(pBB, NULL, &RTV);
    pBB->Release();
    return true;
}

void GraphicsContext::Resize(int w, int h)
{
    ImmediateContext->OMSetRenderTargets(0, 0, 0);
    RTV->Release();
    RTV = nullptr;
    SwapChain->ResizeBuffers(0, w, h, DXGI_FORMAT_UNKNOWN, 0);
    CreateRTV(w, h);
}

void GraphicsContext::SetFullscreen(bool goFull)
{
    EngineSettings::Instance().SetFullscreen(goFull);
    SwapChain->SetFullscreenState(goFull, NULL);
}

ID3DBlob* GraphicsContext::CompileShader(const std::string& src, const std::string& entry, const std::string& profile)
{
    ID3DBlob* blob = nullptr;
    D3DCompile(src.c_str(), src.length(), NULL, NULL, NULL, entry.c_str(), profile.c_str(), 0, 0, &blob, NULL);
    return blob;
}

ShaderSet GraphicsContext::CompileAndCreate(const void* source, size_t length, bool isFile, D3D11_INPUT_ELEMENT_DESC* ied, UINT iedCount)
{
    ShaderSet res;
    ID3DBlob* vsBlob = nullptr;
    ID3DBlob* psBlob = nullptr;
    ID3DBlob* errBlob = nullptr;

    HRESULT hrVS;
    HRESULT hrPS;
    if (isFile)
    {
        hrVS = D3DCompileFromFile((LPCWSTR)source, nullptr, nullptr, "VS", "vs_5_0", 0, 0, &vsBlob, &errBlob);
        hrPS = D3DCompileFromFile((LPCWSTR)source, nullptr, nullptr, "PS", "ps_5_0", 0, 0, &psBlob, &errBlob);
    }
    else
    {
        hrVS = D3DCompile(source, length, nullptr, nullptr, nullptr, "VS", "vs_5_0", 0, 0, &vsBlob, &errBlob);
        hrPS = D3DCompile(source, length, nullptr, nullptr, nullptr, "PS", "ps_5_0", 0, 0, &psBlob, &errBlob);
    }

    if (FAILED(hrVS) || FAILED(hrPS) || !vsBlob || !psBlob)
    {
        if (errBlob)
        {
            OutputDebugStringA((char*)errBlob->GetBufferPointer());
            errBlob->Release();
        }
        if (vsBlob) vsBlob->Release();
        if (psBlob) psBlob->Release();
        return res;
    }

    Device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &res.vs);
    Device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &res.ps);

    if (vsBlob && ied)
    {
        Device->CreateInputLayout(ied, iedCount, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &res.layout);
    }

    if (vsBlob) vsBlob->Release();
    if (psBlob) psBlob->Release();

    return res;
}
