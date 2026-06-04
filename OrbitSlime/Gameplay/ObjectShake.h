#pragma once
#include "Object.h"

class ObjectShake : public Component
{
private:
    float originX;         
    float originY;         
    float shakeDuration;   
    float shakeMagnitude;  

public:
    ObjectShake();
    ~ObjectShake() override;


    void Trigger(float duration, float magnitude);

    void Start(GraphicsContext* gfx) override;
    void Input() override;
    void Update(float dt) override;
    void Render(GraphicsContext* gfx) override;
};
