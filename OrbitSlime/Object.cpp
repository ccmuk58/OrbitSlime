#include "Object.h"

Component::Component()
{
}

Component::~Component()
{
}

GameObject::GameObject(float x, float y, float z)
{
    pos.x = x;
    pos.y = y;
    pos.z = z;
}

GameObject::~GameObject()
{
    for (int i = 0; i < (int)components.size(); i++)
        delete components[i];
}

void GameObject::AddComponent(Component* c)
{
    c->pOwner = this;
    components.push_back(c);
}

void GameObject::Input()
{
    int componentCount = (int)components.size();
    for (int i = 0; i < componentCount; i++)
    {
        if (components[i] != nullptr)
        {
            components[i]->Input();
        }
    }
}

void GameObject::Update(float dt, GraphicsContext* gfx)
{
    for (int j = 0; j < (int)components.size(); j++)
    {
        if (components[j] != nullptr)
        {
            if (components[j]->isStarted == false)
            {
                components[j]->Start(gfx);
                components[j]->isStarted = true;
            }

            components[j]->Update(dt);
        }
    }
}

void GameObject::Render(GraphicsContext* gfx)
{
    for (int i = 0; i < (int)components.size(); i++)
    {
        if (components[i] != nullptr)
        {
            components[i]->Render(gfx);
        }
    }
}
