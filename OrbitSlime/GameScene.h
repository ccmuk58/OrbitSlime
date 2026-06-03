#pragma once

#include "Core.h"
#include "MeshGenerator.h"
#include "Render.h"

class GameLoop;
class GameObject;
class ObjectShake;
class SlimeSquashStretch;

class GameScene
{
    static const UINT DEFAULT_INPUT_COUNT = 3;
    static const UINT UI_INPUT_COUNT = 4;

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
    SlimeSquashStretch* slimeSquash = nullptr;

    void SetupInputLayouts();
    void CreateStarField(GameLoop& gEngine);
    void CreatePlanet(GameLoop& gEngine);
    void CreateSlime(GameLoop& gEngine);
    void CreateAsteroids(GameLoop& gEngine);
    void CreateParticles(GameLoop& gEngine);

public:
    void LoadResources(GameLoop& gEngine);
    void CreateWorld(GameLoop& gEngine);
    void CreateUI(GameLoop& gEngine);
    void ReleaseResources();
};
