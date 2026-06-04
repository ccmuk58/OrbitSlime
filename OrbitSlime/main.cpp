










#include "GameLoop.h"
#include "AsteroidMovement.h"
#include "TrailRenderer.h"
#include "BitmapTextRenderer.h"
#include "CircleCollider.h"
#include "MeshGenerator.h"
#include "Object.h"
#include "ObjectShake.h"
#include "Particle.h"
#include "PlanetHealthVisual.h"
#include "PlayerController.h"
#include "SlimePhysics.h"
#include "StarFieldRenderer.h"
#include "TimerDisplay.h"

#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:windows")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

const UINT DEFAULT_INPUT_COUNT = 3;
const UINT UI_INPUT_COUNT = 4;

D3D11_INPUT_ELEMENT_DESC defaultInputLayout[DEFAULT_INPUT_COUNT] = {};
D3D11_INPUT_ELEMENT_DESC uiInputLayout[UI_INPUT_COUNT] = {};

float planetRadius = 0.3f;
float slimeRadius = 0.1f;

MeshData planetMeshData;
MeshData slimeMeshData;
MeshData asteroidMeshData;

Mesh* timerMesh = nullptr;
Mesh* titleTextMesh = nullptr;
Mesh* gameOverTextMesh = nullptr;
Mesh* planetMesh = nullptr;
Mesh* asteroidMesh = nullptr;
Mesh* slimeMesh = nullptr;
Mesh* particleMesh = nullptr;

ShaderSet fontShader;
ShaderSet planetShader;
ShaderSet asteroidShader;
ShaderSet slimeShader;
ShaderSet starShader;

TextureMaterial* fontMat = nullptr;
ColorMaterial* planetMat = nullptr;
ColorMaterial* asteroidMat = nullptr;
ColorMaterial* asteroidTrailMat = nullptr;
ColorMaterial* slimeMat = nullptr;
ColorMaterial* slimeTrailMat = nullptr;
ColorMaterial* starMat = nullptr;
ColorMaterial* particleMat = nullptr;

GameObject* starField = nullptr;
GameObject* planet = nullptr;
GameObject* slime = nullptr;
GameObject* timerObject = nullptr;
GameObject* titleTextObject = nullptr;
GameObject* gameOverTextObject = nullptr;

ObjectShake* planetShake = nullptr;
SlimePhysics* slimePhysics = nullptr;

LRESULT CALLBACK GlobalWndProc(HWND h, UINT m, WPARAM w, LPARAM l)
{
	if (m == WM_DESTROY) PostQuitMessage(0);
	return DefWindowProc(h, m, w, l);
}

void SetupWindowSettings()
{
	
	EngineSettings& settings = EngineSettings::Instance();
	settings.SetWindowTitle(L"Orbit Slime");
	settings.SetWindowClassName(L"Orbit Slime");
	settings.SetWindowSize(1280, 720);
	settings.SetResizedWindowSize(1280, 720);
	settings.SetVSync(true);
	settings.SetClearColor(0.02f, 0.025f, 0.045f, 1.0f);
}

void SetupInputLayouts()
{
	defaultInputLayout[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	defaultInputLayout[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	defaultInputLayout[2] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 };

	uiInputLayout[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	uiInputLayout[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	uiInputLayout[2] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	uiInputLayout[3] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 };
}

void LoadResources(GameLoop& gEngine)
{
	SetupInputLayouts();

	
	planetMeshData = MeshGenerator::CreateIrregularSphere(planetRadius, 20, 20, 0.05f);
	slimeMeshData = MeshGenerator::CreateHemiSphere(slimeRadius, 20, 20);
	asteroidMeshData = MeshGenerator::CreateIrregularSphere(0.05f, 14, 10, 0.32f);

	starShader = gEngine.gfx.CompileAndCreate(L"Shaders\\StarField.hlsl", 0, true, defaultInputLayout, DEFAULT_INPUT_COUNT);
	starMat = new ColorMaterial(starShader, { 0.85f, 0.9f, 1.0f, 1.0f }, gEngine.gfx.Device);
	starMat->SetSpecular(0.0f, 1.0f);

	fontShader = gEngine.gfx.CompileAndCreate(L"Shaders\\DigitTexture.hlsl", 0, true, uiInputLayout, UI_INPUT_COUNT);
	fontMat = new TextureMaterial(fontShader, L"Assets\\newFont.png", gEngine.gfx.Device);
	fontMat->SetAlphaBlend(true);

	planetMesh = new Mesh();
	planetMesh->Create(&gEngine.gfx, planetMeshData.vertices, planetMeshData.indices);
	planetShader = gEngine.gfx.CompileAndCreate(L"Shaders\\Planet.hlsl", 0, true, defaultInputLayout, DEFAULT_INPUT_COUNT);
	planetMat = new ColorMaterial(planetShader, { 0.4f, 0.25f, 0.15f, 1.0f }, gEngine.gfx.Device);
	planetMat->SetSpecular(0.5f, 32.0f);

	asteroidMesh = new Mesh();
	asteroidMesh->Create(&gEngine.gfx, asteroidMeshData.vertices, asteroidMeshData.indices);
	asteroidShader = gEngine.gfx.CompileAndCreate(L"Shaders\\Asteroid.hlsl", 0, true, defaultInputLayout, DEFAULT_INPUT_COUNT);
	asteroidMat = new ColorMaterial(asteroidShader, { 0.1f, 0.1f, 0.1f, 1.0f }, gEngine.gfx.Device);
	asteroidMat->SetSpecular(0.3f, 16.0f);

	asteroidTrailMat = new ColorMaterial(asteroidShader, { 1.0f, 0.28f, 0.05f, 0.18f }, gEngine.gfx.Device);
	asteroidTrailMat->SetSpecular(0.0f, 1.0f);
	asteroidTrailMat->SetAlphaBlend(true);

	slimeMesh = new Mesh();
	slimeMesh->Create(&gEngine.gfx, slimeMeshData.vertices, slimeMeshData.indices);
	slimeShader = gEngine.gfx.CompileAndCreate(L"Shaders\\Slime.hlsl", 0, true, defaultInputLayout, DEFAULT_INPUT_COUNT);
	slimeMat = new ColorMaterial(slimeShader, { 0.1f, 0.8f, 0.3f, 1.0f }, gEngine.gfx.Device);
	slimeMat->SetSpecular(0.25f, 12.0f);
	slimeMat->SetAlphaBlend(true);

	slimeTrailMat = new ColorMaterial(slimeShader, { 0.1f, 0.9f, 0.3f, 0.18f }, gEngine.gfx.Device);
	slimeTrailMat->SetSpecular(0.0f, 1.0f);
	slimeTrailMat->SetAlphaBlend(true);

	MeshData particleData = MeshGenerator::CreateSphere(1.0f, 6, 6);
	particleMesh = new Mesh();
	particleMesh->Create(&gEngine.gfx, particleData.vertices, particleData.indices);
	particleMat = new ColorMaterial(asteroidShader, { 1.0f, 0.4f, 0.0f, 1.0f }, gEngine.gfx.Device);
	particleMat->SetSpecular(0.0f, 1.0f);
}

void CreateStarField(GameLoop& gEngine)
{
	starField = new GameObject(0, 0, 0);
	starField->AddComponent(new StarFieldRenderer(starMat, 128 * 2, 5.3f * 2, 3.0f, 0.01f));
	gEngine.world.push_back(starField);
}

void CreatePlanet(GameLoop& gEngine)
{
	planet = new GameObject(0, 0, 0);
	planet->AddComponent(new MeshRenderer(planetMesh, planetMat));
	planet->AddComponent(new PlanetHealthVisual(planetMat, 5));

	planetShake = new ObjectShake();
	planet->AddComponent(planetShake);
}

void CreateSlime(GameLoop& gEngine)
{
	slime = new GameObject(0, 0, 0);

	TrailRenderer* slimeTrail = new TrailRenderer(slimeMesh, slimeTrailMat, 5, TrailType::Slime);
	slimeTrail->isEmitting = false;
	slime->AddComponent(slimeTrail);

	slime->AddComponent(new MeshRenderer(slimeMesh, slimeMat));
	PlayerController* slimeController = new PlayerController(planet, planetRadius - 0.015f, 2.5f);
	slimeController->trail = slimeTrail;
	slime->AddComponent(slimeController);

	slimePhysics = new SlimePhysics(slimeController);
	slime->AddComponent(slimePhysics);
}

void CreateAsteroids(GameLoop& gEngine)
{
	const int ASTEROID_COUNT = 10;
	for (int i = 0; i < ASTEROID_COUNT; i++)
	{
		GameObject* asteroid = new GameObject(0, 0, 0);

		asteroid->AddComponent(new TrailRenderer(asteroidMesh, asteroidTrailMat, 25, TrailType::Asteroid));
		asteroid->AddComponent(new MeshRenderer(asteroidMesh, asteroidMat));

		AsteroidMovement* astMove = new AsteroidMovement(planet, 0.0f, planetShake);
		asteroid->AddComponent(astMove);
		gEngine.asteroids.push_back(astMove);

		asteroid->AddComponent(new CircleCollider(slime, 0.05f, 0.1f, astMove, slimePhysics));

		gEngine.world.push_back(asteroid);
		gEngine.pointLightObjects.push_back(asteroid);
	}
}

void CreateParticles(GameLoop& gEngine)
{
	for (int i = 0; i < 40; i++)
	{
		GameObject* pObj = new GameObject(0, 0, 0);
		pObj->isActive = false;

		pObj->AddComponent(new MeshRenderer(particleMesh, particleMat));

		ParticleComponent* pComp = new ParticleComponent();
		pObj->AddComponent(pComp);

		gEngine.world.push_back(pObj);
		ParticleManager::pool.push_back(pComp);
	}
}

void CreateWorld(GameLoop& gEngine)
{
	CreateStarField(gEngine);
	CreatePlanet(gEngine);
	CreateSlime(gEngine);
	CreateAsteroids(gEngine);
	CreateParticles(gEngine);

	gEngine.world.push_back(planet);
	gEngine.world.push_back(slime);
}

void CreateUI(GameLoop& gEngine)
{
	timerMesh = new Mesh();
	titleTextMesh = new Mesh();
	gameOverTextMesh = new Mesh();

	timerObject = new GameObject(0.0f, 0.72f, 0.0f);
	TimerDisplay* timerDisplay = new TimerDisplay(timerMesh, 0.07f, 0.12f, 0.002f);
	timerObject->AddComponent(new MeshRenderer(timerMesh, fontMat));
	timerObject->AddComponent(timerDisplay);
	gEngine.uiTimer = timerDisplay;

	titleTextObject = new GameObject(0.0f, -0.62f, 0.0f);
	BitmapTextRenderer* titleText = new BitmapTextRenderer(titleTextMesh, 0.055f, 0.10f, 0.0015f);
	titleText->SetDisplayText("Press 'Space' to start");
	titleTextObject->AddComponent(new MeshRenderer(titleTextMesh, fontMat));
	titleTextObject->AddComponent(titleText);
	gEngine.uiTitle = titleTextObject;

	gameOverTextObject = new GameObject(0.0f, -0.62f, 0.0f);
	BitmapTextRenderer* gameOverText = new BitmapTextRenderer(gameOverTextMesh, 0.055f, 0.10f, 0.0015f);
	gameOverText->SetDisplayText("Press 'R' to restart");
	gameOverTextObject->AddComponent(new MeshRenderer(gameOverTextMesh, fontMat));
	gameOverTextObject->AddComponent(gameOverText);
	gEngine.uiGameOver = gameOverTextObject;

	gEngine.world.push_back(timerObject);
	gEngine.world.push_back(titleTextObject);
	gEngine.world.push_back(gameOverTextObject);
}

void ReleaseResources()
{
	if (slimeMat) { delete slimeMat; slimeMat = nullptr; }
	if (asteroidMat) { delete asteroidMat; asteroidMat = nullptr; }
	if (asteroidTrailMat) { delete asteroidTrailMat; asteroidTrailMat = nullptr; }
	if (planetMat) { delete planetMat; planetMat = nullptr; }
	if (starMat) { delete starMat; starMat = nullptr; }
	if (particleMat) { delete particleMat; particleMat = nullptr; }
	if (fontMat) { delete fontMat; fontMat = nullptr; }
	if (slimeTrailMat) { delete slimeTrailMat; slimeTrailMat = nullptr; }

	slimeShader.Release();
	asteroidShader.Release();
	planetShader.Release();
	starShader.Release();
	fontShader.Release();

	if (slimeMesh) { delete slimeMesh; slimeMesh = nullptr; }
	if (asteroidMesh) { delete asteroidMesh; asteroidMesh = nullptr; }
	if (planetMesh) { delete planetMesh; planetMesh = nullptr; }
	if (particleMesh) { delete particleMesh; particleMesh = nullptr; }
	if (timerMesh) { delete timerMesh; timerMesh = nullptr; }
	if (titleTextMesh) { delete titleTextMesh; titleTextMesh = nullptr; }
	if (gameOverTextMesh) { delete gameOverTextMesh; gameOverTextMesh = nullptr; }
}

int WINAPI WinMain(HINSTANCE hI, HINSTANCE, LPSTR, int nS)
{
	SetupWindowSettings();

	
	GameLoop gEngine;
	gEngine.Initialize(hI, GlobalWndProc);

	
	LoadResources(gEngine);
	
	CreateWorld(gEngine);
	
	CreateUI(gEngine);

	
	gEngine.Run();

	ReleaseResources();
	return 0;
}
