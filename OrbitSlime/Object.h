#pragma once

#include "Core.h"

#include <directxmath.h>
#include <vector>

using namespace DirectX;

class GameObject;

class Component
{
public:
    GameObject* pOwner = nullptr;
    bool isStarted = false;

    Component();
    virtual ~Component();

    virtual void Start(GraphicsContext* gfx) = 0;
    virtual void Input() = 0;
    virtual void Update(float dt) = 0;
    virtual void Render(GraphicsContext* gfx) = 0;
};

class GameObject
{
public:
    XMFLOAT3 pos = { 0, 0, 0 };
    XMFLOAT3 rot = { 0, 0, 0 };
    XMFLOAT3 scale = { 1, 1, 1 };
    std::vector<Component*> components;

    GameObject(float x, float y, float z);
    ~GameObject();

    void AddComponent(Component* c);
    void Input();
    void Update(float dt, GraphicsContext* gfx);
    void Render(GraphicsContext* gfx);
};
