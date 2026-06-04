#include "Material.h"
#include "Logger.h"

#include <cstdio>
#include <cwchar>
#include <vector>
#include <wincodec.h>

#pragma comment(lib, "windowscodecs.lib")

static DXGI_FORMAT GetWICFormat(WICPixelFormatGUID pixelFormat)
{
    
    if (pixelFormat == GUID_WICPixelFormat32bppRGBA) return DXGI_FORMAT_R8G8B8A8_UNORM;
    if (pixelFormat == GUID_WICPixelFormat32bppBGRA) return DXGI_FORMAT_B8G8R8A8_UNORM;
    return DXGI_FORMAT_UNKNOWN;
}

static bool LoadTextureFromFile(
    ID3D11Device* device,
    const wchar_t* filePath,
    ID3D11ShaderResourceView** outSRV)
{
    if (!device || !filePath || !outSRV) return false;

    *outSRV = nullptr;

    
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    IWICImagingFactory* factory = nullptr;
    IWICBitmapDecoder* decoder = nullptr;
    IWICBitmapFrameDecode* frame = nullptr;
    IWICFormatConverter* converter = nullptr;

    HRESULT hr = CoCreateInstance(
        CLSID_WICImagingFactory,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&factory));

    if (SUCCEEDED(hr))
    {
        
        hr = factory->CreateDecoderFromFilename(
            filePath,
            nullptr,
            GENERIC_READ,
            WICDecodeMetadataCacheOnLoad,
            &decoder);
    }

    if (SUCCEEDED(hr)) hr = decoder->GetFrame(0, &frame);

    UINT width = 0;
    UINT height = 0;
    if (SUCCEEDED(hr)) hr = frame->GetSize(&width, &height);

    WICPixelFormatGUID sourceFormat = {};
    if (SUCCEEDED(hr)) hr = frame->GetPixelFormat(&sourceFormat);

    IWICBitmapSource* bitmapSource = frame;
    DXGI_FORMAT textureFormat = GetWICFormat(sourceFormat);

    if (SUCCEEDED(hr) && textureFormat == DXGI_FORMAT_UNKNOWN)
    {
        
        hr = factory->CreateFormatConverter(&converter);
        if (SUCCEEDED(hr))
        {
            hr = converter->Initialize(
                frame,
                GUID_WICPixelFormat32bppRGBA,
                WICBitmapDitherTypeNone,
                nullptr,
                0.0,
                WICBitmapPaletteTypeCustom);
            bitmapSource = converter;
            textureFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        }
    }

    std::vector<unsigned char> pixels;
    if (SUCCEEDED(hr))
    {
        
        pixels.resize((size_t)width * (size_t)height * 4);
        hr = bitmapSource->CopyPixels(nullptr, width * 4, (UINT)pixels.size(), pixels.data());
    }

    ID3D11Texture2D* texture = nullptr;
    if (SUCCEEDED(hr))
    {
        
        D3D11_TEXTURE2D_DESC desc = {};
        desc.Width = width;
        desc.Height = height;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = textureFormat;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

        D3D11_SUBRESOURCE_DATA initData = {};
        initData.pSysMem = pixels.data();
        initData.SysMemPitch = width * 4;

        hr = device->CreateTexture2D(&desc, &initData, &texture);
    }

    if (SUCCEEDED(hr))
    {
        
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = textureFormat;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;
        hr = device->CreateShaderResourceView(texture, &srvDesc, outSRV);
    }

    if (texture) texture->Release();
    if (converter) converter->Release();
    if (frame) frame->Release();
    if (decoder) decoder->Release();
    if (factory) factory->Release();

    return SUCCEEDED(hr);
}

Material::Material(ShaderSet s)
    : shaders(s)
{
}

Material::~Material()
{
}

ColorMaterial::ColorMaterial(ShaderSet s, XMFLOAT4 col, ID3D11Device* device)
    : Material(s), color(col)
{
    D3D11_BUFFER_DESC cbd = { 0 };
    cbd.Usage = D3D11_USAGE_DEFAULT;
    cbd.ByteWidth = sizeof(ColorBuffer);
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    device->CreateBuffer(&cbd, nullptr, &pColorBuffer);
}

ColorMaterial::~ColorMaterial()
{
    if (pColorBuffer) pColorBuffer->Release();
}

void ColorMaterial::SetColor(XMFLOAT4 col)
{
    color = col;
}

void ColorMaterial::SetSpecular(float strength, float power)
{
    specularStrength = strength;
    specularPower = power;
}

void ColorMaterial::SetAlphaBlend(bool enabled)
{
    useAlphaBlend = enabled;
}

bool ColorMaterial::UseAlphaBlend() const
{
    return useAlphaBlend;
}

void ColorMaterial::Bind(ID3D11DeviceContext* context)
{
    context->IASetInputLayout(shaders.layout);
    context->VSSetShader(shaders.vs, nullptr, 0);
    context->PSSetShader(shaders.ps, nullptr, 0);

    ColorBuffer cb;
    cb.tintColor = color;
    cb.specularStrength = specularStrength;
    cb.specularPower = specularPower;
    cb.padding = { 0.0f, 0.0f };

    context->UpdateSubresource(pColorBuffer, 0, nullptr, &cb, 0, 0);
    context->PSSetConstantBuffers(1, 1, &pColorBuffer);
}

TextureMaterial::TextureMaterial(ShaderSet s, const wchar_t* filePath, ID3D11Device* device)
    : Material(s), pSRV(nullptr), pSampler(nullptr)
{
    
    bool loaded = LoadTextureFromFile(device, filePath, &pSRV);

    if (!loaded)
    {
        wchar_t exePath[MAX_PATH] = {};
        GetModuleFileNameW(nullptr, exePath, MAX_PATH);

        wchar_t* lastSlash = wcsrchr(exePath, L'\\');
        if (lastSlash)
        {
            *(lastSlash + 1) = L'\0';

            wchar_t exeRelativePath[MAX_PATH] = {};
            swprintf_s(exeRelativePath, L"%s%s", exePath, filePath);
            loaded = LoadTextureFromFile(device, exeRelativePath, &pSRV);
        }
    }

    if (!loaded)
    {
        wchar_t projectRelativePath[MAX_PATH] = {};
        swprintf_s(projectRelativePath, L"OrbitSlime\\%s", filePath);
        loaded = LoadTextureFromFile(device, projectRelativePath, &pSRV);
    }

    if (!loaded)
    {
        Logger::LogWideFormat(L"[TextureMaterial] Failed to load texture: %s", filePath);
    }

    D3D11_SAMPLER_DESC samplerDesc = {};

    
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    device->CreateSamplerState(&samplerDesc, &pSampler);
}

TextureMaterial::~TextureMaterial()
{
    if (pSampler) pSampler->Release();
    if (pSRV) pSRV->Release();
}

void TextureMaterial::SetAlphaBlend(bool enabled)
{
    useAlphaBlend = enabled;
}

bool TextureMaterial::UseAlphaBlend() const
{
    return useAlphaBlend;
}

void TextureMaterial::Bind(ID3D11DeviceContext* context)
{
    
    context->IASetInputLayout(shaders.layout);
    context->VSSetShader(shaders.vs, nullptr, 0);
    context->PSSetShader(shaders.ps, nullptr, 0);
    context->PSSetShaderResources(0, 1, &pSRV);
    context->PSSetSamplers(0, 1, &pSampler);
}
