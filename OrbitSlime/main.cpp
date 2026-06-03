/*
================================================================================
 [Engine Architecture]
 1. WindowContext: Win32 Ã¢ »ı¼º ¹× ¸Ş½ÃÁö ·çÇÁ °ü¸®
 2. GraphicsContext: DX11 µğ¹ÙÀÌ½º, ½º¿ÒÃ¼ÀÎ, ¼ÎÀÌ´õ ÄÄÆÄÀÏ ¹× ¿µ»ó ¼³Á¤ °ü¸®
 3. DeltaTime: °íÇØ»óµµ Å¸ÀÌ¸Ó¸¦ ÀÌ¿ëÇÑ ½Ã°£ °è»ê
 4. GameObject & Component: °´Ã¼ ÁöÇâÀû ±â´É È®Àå ±¸Á¶
 5. GameLoop: ÀüÃ¼ Èå¸§(Input-Update-Render) Á¦¾î
================================================================================
*/

#include "GameLoop.h"
#include "Object.h"
#include "PlayerController.h"
#include "Render.h"
#include "MeshGenerator.h"
#include "AsteroidMovement.h"
#include "AsteroidTrailRenderer.h"
#include "StarFieldRenderer.h"
#include "CircleCollider.h"
#include "ObjectShake.h"
#include "Particle.h"
#include "TimerDisplay.h"
#include "SlimeSquashStretch.h"
#include "PlanetHealthVisual.h"
#include <d3dcompiler.h>
#include <vector>


#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")


std::vector<ParticleComponent*> ParticleManager::pool;

LRESULT CALLBACK GlobalWndProc(HWND h, UINT m, WPARAM w, LPARAM l)
{
    if (m == WM_DESTROY) PostQuitMessage(0);
    return DefWindowProc(h, m, w, l);
}

int WINAPI WinMain(HINSTANCE hI, HINSTANCE, LPSTR, int nS)
{
    // À©µµ¿ì ¼¼ÆÃ
    EngineSettings& settings = EngineSettings::Instance();
    settings.SetWindowTitle(L"Orbit Slime");
    settings.SetWindowClassName(L"Orbit Slime");
    settings.SetWindowSize(1600, 900);
    settings.SetResizedWindowSize(1600, 900);
    settings.SetVSync(true);
    settings.SetClearColor(0.02f, 0.025f, 0.045f, 1.0f);

	// °ÔÀÓ·çÇÁ »ı¼º ¹× ÃÊ±âÈ­
    GameLoop gEngine;
    gEngine.Initialize(hI, GlobalWndProc);

	// ¼ÎÀÌ´õ ÀÔ·Â ·¹ÀÌ¾Æ¿ô Á¤ÀÇ
	const UINT iedCount = 3;
    D3D11_INPUT_ELEMENT_DESC ied[iedCount] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    const UINT uiIedCount = 4;
    D3D11_INPUT_ELEMENT_DESC uiIed[uiIedCount] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

	// ¸Ş½¬ µ¥ÀÌÅÍ »ı¼º
	float planetRadius = 0.3f;
	float slimeRadius = 0.1f;
    // MeshData planetMeshData = MeshGenerator::CreateSphere(planetRadius, 20, 20);
    MeshData planetMeshData = MeshGenerator::CreateIrregularSphere(planetRadius, 20, 20, 0.05f);
    MeshData slimeMeshData = MeshGenerator::CreateHemiSphere(slimeRadius, 20, 20);
    MeshData asteroidMeshData = MeshGenerator::CreateIrregularSphere(0.05f, 14, 10, 0.32f);


    // ¹è°æ º°
    ShaderSet starShader = gEngine.gfx.CompileAndCreate(L"StarField.hlsl", 0, true, ied, iedCount);
    ColorMaterial* starMat = new ColorMaterial(starShader, { 0.85f, 0.9f, 1.0f, 1.0f }, gEngine.gfx.Device);
    starMat->SetSpecular(0.0f, 1.0f);
    GameObject* starField = new GameObject(0, 0, 0);
    starField->AddComponent(new StarFieldRenderer(starMat, 128*2, 5.3f*2, 3.0f, 0.01f));

    // numbers.png¸¦ Àß¶ó¼­ ¾²´Â È­¸é »ó´Ü Å¸ÀÌ¸Ó
    Mesh* timerMesh = new Mesh();
    ShaderSet digitShader = gEngine.gfx.CompileAndCreate(L"DigitTexture.hlsl", 0, true, uiIed, uiIedCount);
    TextureMaterial* timerMat = new TextureMaterial(digitShader, L"numbers.png", gEngine.gfx.Device);
    timerMat->SetAlphaBlend(true);
    GameObject* timerObject = new GameObject(0.0f, 0.72f, 0.0f);
    TimerDisplay* timerDisplay = new TimerDisplay(timerMesh, 0.2f, 0.16f, 0.01f);
    timerObject->AddComponent(new MeshRenderer(timerMesh, timerMat));
    timerObject->AddComponent(timerDisplay);
    gEngine.uiTimer = timerDisplay;

	// Çà¼º
    Mesh* planetMesh = new Mesh();
    planetMesh->Create(&gEngine.gfx, planetMeshData.vertices, planetMeshData.indices);
    ShaderSet planetShader = gEngine.gfx.CompileAndCreate(L"Planet.hlsl", 0, true, ied, iedCount);
    ColorMaterial* planetMat = new ColorMaterial(planetShader, { 0.4f, 0.25f, 0.15f, 1 }, gEngine.gfx.Device);
	planetMat->SetSpecular(0.5f, 32.0f);
    GameObject* planet = new GameObject(0, 0, 0);
    planet->AddComponent(new MeshRenderer(planetMesh, planetMat));
    planet->AddComponent(new PlanetHealthVisual(planetMat, 5));

    ObjectShake* planetShake = new ObjectShake();
    planet->AddComponent(planetShake);

    // ¼ÒÇà¼º
    Mesh* asteroidMesh = new Mesh();
    asteroidMesh->Create(&gEngine.gfx, asteroidMeshData.vertices, asteroidMeshData.indices);
    ShaderSet asteroidShader = gEngine.gfx.CompileAndCreate(L"Asteroid.hlsl", 0, true, ied, iedCount);
    ColorMaterial* asteroidMat = new ColorMaterial(asteroidShader, { 0.1f, 0.1f, 0.1f, 1 }, gEngine.gfx.Device);
	asteroidMat->SetSpecular(0.3f, 16.0f);
    // ¼ÒÇà¼º ÀÜ»ó ¸ÓÆ¼¸®¾ó
    ColorMaterial* asteroidTrailMat = new ColorMaterial(asteroidShader, { 1.0f, 0.28f, 0.05f, 0.18f }, gEngine.gfx.Device);
    asteroidTrailMat->SetSpecular(0.0f, 1.0f);
    asteroidTrailMat->SetAlphaBlend(true);

    // ½½¶óÀÓ
    Mesh* slimeMesh = new Mesh();
    slimeMesh->Create(&gEngine.gfx, slimeMeshData.vertices, slimeMeshData.indices);
    ShaderSet slimeShader = gEngine.gfx.CompileAndCreate(L"Slime.hlsl", 0, true, ied, iedCount);

    ColorMaterial* slimeMat = new ColorMaterial(slimeShader, { 0.1f, 0.8f, 0.3f, 0.2f }, gEngine.gfx.Device);
    slimeMat->SetSpecular(0.25f, 12.0f);
    slimeMat->SetAlphaBlend(true);
    // ½½¶óÀÓ ÀÜ»ó ¸ÓÆ¼¸®¾ó
    ColorMaterial* slimeTrailMat = new ColorMaterial(slimeShader, { 0.1f, 0.9f, 0.3f, 0.18f }, gEngine.gfx.Device);
    slimeTrailMat->SetSpecular(0.0f, 1.0f);
    slimeTrailMat->SetAlphaBlend(true);

    GameObject* slime = new GameObject(0, 0, 0);

    AsteroidTrailRenderer* slimeTrail = new AsteroidTrailRenderer(slimeMesh, slimeTrailMat);
    slimeTrail->isEmitting = false;
    slime->AddComponent(slimeTrail);

    slime->AddComponent(new MeshRenderer(slimeMesh, slimeMat));
    PlayerController* slimeController = new PlayerController(planet, planetRadius, 2.5f);
    slimeController->trail = slimeTrail;
    slime->AddComponent(slimeController);
    SlimeSquashStretch* slimeSquash = new SlimeSquashStretch(slimeController);
    slime->AddComponent(slimeSquash);

    gEngine.world.push_back(starField);

    // ¼ÒÇà¼º 10°³ ¼ÒÈ¯
    const int ASTEROID_COUNT = 10;
    for (int i = 0; i < ASTEROID_COUNT; i++)
    {
        // À§Ä¡, Å©±â ¸ğµÎ ¾Ë¾Æ¼­ ÇÒ Å×´Ï ÀÏ´Ü ´ëÃæ »ı¼º
        GameObject* asteroid = new GameObject(0, 0, 0);

        // ¼ÒÇà¼º ÀÜ»ó ÄÄÆ÷³ÍÆ® Ãß°¡
        asteroid->AddComponent(new AsteroidTrailRenderer(asteroidMesh, asteroidTrailMat));
        asteroid->AddComponent(new MeshRenderer(asteroidMesh, asteroidMat));

        // ¼Óµµµµ  ·£´ıÀ¸·Î Á¤ÇÒ °Å´Ï±î ºóÄ­À¸·Î ³Ö±â
        AsteroidMovement* astMove = new AsteroidMovement(planet, 0.0f, planetShake);
        asteroid->AddComponent(astMove);

        gEngine.asteroids.push_back(astMove);

        asteroid->AddComponent(new CircleCollider(slime, 0.05f, 0.1f, astMove, slimeSquash));

        gEngine.world.push_back(asteroid);
        gEngine.pointLightObjects.push_back(asteroid);
    }
    MeshData particleData = MeshGenerator::CreateIrregularSphere(1.0f, 6, 6, 0.0f);
    Mesh* particleMesh = new Mesh();
    particleMesh->Create(&gEngine.gfx, particleData.vertices, particleData.indices);

    // ?Œí¸ ?‰ìƒ?€ ?©ì•”/ë¶ˆê½ƒ ?ë‚Œ??ì£¼í™©+ë¹¨ê°•! (asteroidShader ?¬ì‚¬??
    ColorMaterial* particleMat = new ColorMaterial(asteroidShader, { 1.0f, 0.4f, 0.0f, 1.0f }, gEngine.gfx.Device);
    particleMat->SetSpecular(0.0f, 1.0f);

    for (int i = 0; i < 60; i++)
    {
        GameObject* pObj = new GameObject(0, 0, 0);
        pObj->isActive = false; //?œì¼ ì¤‘ìš”! ?‰ì†Œ???¬ëª…?˜ê²Œ ?¨ê²¨??

        pObj->AddComponent(new MeshRenderer(particleMesh, particleMat));

        ParticleComponent* pComp = new ParticleComponent();
        pObj->AddComponent(pComp);

        gEngine.world.push_back(pObj);
        ParticleManager::pool.push_back(pComp); // ë§¤ë‹ˆ?€???±ë¡
    }
    gEngine.world.push_back(planet);
    gEngine.world.push_back(slime);
    gEngine.world.push_back(timerObject);

	// °ÔÀÓ ·çÇÁ ½ÃÀÛ
    gEngine.Run();


    if (slimeMat) { delete slimeMat; slimeMat = nullptr; }
	if (asteroidMat) { delete asteroidMat; asteroidMat = nullptr; }
    if (asteroidTrailMat) { delete asteroidTrailMat; asteroidTrailMat = nullptr; }
	if (planetMat) { delete planetMat; planetMat = nullptr; }
    if (starMat) { delete starMat; starMat = nullptr; }
    if (particleMat) { delete particleMat; particleMat = nullptr; }
    if (timerMat) { delete timerMat; timerMat = nullptr; }

    slimeShader.Release();
	asteroidShader.Release();
	planetShader.Release();
    starShader.Release();
    digitShader.Release();

    if (slimeMesh) { delete slimeMesh; slimeMesh = nullptr; }
	if (asteroidMesh) { delete asteroidMesh; asteroidMesh = nullptr; }
	if (planetMesh) { delete planetMesh; planetMesh = nullptr; }
    if (particleMesh) { delete particleMesh; particleMesh = nullptr; }
    if (timerMesh) { delete timerMesh; timerMesh = nullptr; }
    return 0;
}
