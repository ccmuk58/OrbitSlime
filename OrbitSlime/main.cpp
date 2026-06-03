/*
================================================================================
 [Engine Architecture]
 1. WindowContext: Win32 창 생성 및 메시지 루프 관리
 2. GraphicsContext: DX11 디바이스, 스왑체인, 셰이더 컴파일 및 영상 설정 관리
 3. DeltaTime: 고해상도 타이머를 이용한 시간 계산
 4. GameObject & Component: 객체 지향적 기능 확장 구조
 5. GameLoop: 전체 흐름(Input-Update-Render) 제어
================================================================================
*/

#include "GameLoop.h"
#include "GameScene.h"

#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:windows")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

LRESULT CALLBACK GlobalWndProc(HWND h, UINT m, WPARAM w, LPARAM l)
{
	if (m == WM_DESTROY) PostQuitMessage(0);
	return DefWindowProc(h, m, w, l);
}

void SetupWindowSettings()
{
	// 윈도우 세팅
	EngineSettings& settings = EngineSettings::Instance();
	settings.SetWindowTitle(L"Orbit Slime");
	settings.SetWindowClassName(L"Orbit Slime");
	settings.SetWindowSize(1280, 720);
	settings.SetResizedWindowSize(1280, 720);
	settings.SetVSync(true);
	settings.SetClearColor(0.02f, 0.025f, 0.045f, 1.0f);
}

int WINAPI WinMain(HINSTANCE hI, HINSTANCE, LPSTR, int nS)
{
	SetupWindowSettings();

	// 게임루프 생성 및 초기화
	GameLoop gEngine;
	gEngine.Initialize(hI, GlobalWndProc);

	GameScene gameScene;
	// 셰이더, 메쉬, 머티리얼 생성
	gameScene.LoadResources(gEngine);
	// 행성, 슬라임, 소행성, 배경 생성
	gameScene.CreateWorld(gEngine);
	// 타이머와 시작/종료 안내 문구 생성
	gameScene.CreateUI(gEngine);

	// 게임 루프 시작
	gEngine.Run();

	gameScene.ReleaseResources();
	return 0;
}
