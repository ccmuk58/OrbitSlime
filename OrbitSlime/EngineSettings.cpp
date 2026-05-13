#include "EngineSettings.h"

EngineSettings& EngineSettings::Instance()
{
    static EngineSettings settings;
    return settings;
}

int EngineSettings::GetWindowWidth() const
{
    return windowWidth;
}

int EngineSettings::GetWindowHeight() const
{
    return windowHeight;
}

void EngineSettings::SetWindowSize(int width, int height)
{
    windowWidth = width;
    windowHeight = height;
}

int EngineSettings::GetResizedWindowWidth() const
{
    return resizedWindowWidth;
}

int EngineSettings::GetResizedWindowHeight() const
{
    return resizedWindowHeight;
}

void EngineSettings::SetResizedWindowSize(int width, int height)
{
    resizedWindowWidth = width;
    resizedWindowHeight = height;
}

bool EngineSettings::IsFullscreen() const
{
    return fullscreen;
}

void EngineSettings::SetFullscreen(bool value)
{
    fullscreen = value;
}

void EngineSettings::ToggleFullscreen()
{
    fullscreen = !fullscreen;
}

bool EngineSettings::IsVSyncEnabled() const
{
    return vSync;
}

UINT EngineSettings::GetPresentInterval() const
{
    return vSync ? 1 : 0;
}

void EngineSettings::SetVSync(bool value)
{
    vSync = value;
}

const float* EngineSettings::GetClearColor() const
{
    return clearColor;
}

void EngineSettings::SetClearColor(float r, float g, float b, float a)
{
    clearColor[0] = r;
    clearColor[1] = g;
    clearColor[2] = b;
    clearColor[3] = a;
}

LPCWSTR EngineSettings::GetWindowTitle() const
{
    return windowTitle;
}

void EngineSettings::SetWindowTitle(LPCWSTR title)
{
    windowTitle = title;
}

LPCWSTR EngineSettings::GetWindowClassName() const
{
    return windowClassName;
}

void EngineSettings::SetWindowClassName(LPCWSTR className)
{
    windowClassName = className;
}
