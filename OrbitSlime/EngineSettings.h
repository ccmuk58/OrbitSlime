#pragma once

#include <windows.h>

class EngineSettings
{
    int windowWidth = 800;
    int windowHeight = 800;
    int resizedWindowWidth = 1600;
    int resizedWindowHeight = 900;
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
