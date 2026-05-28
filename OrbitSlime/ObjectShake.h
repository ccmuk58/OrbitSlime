#pragma once
#include "Object.h"

class ObjectShake : public Component
{
private:
    float originX;         // 원래 있던 원래 X 위치
    float originY;         // 원래 있던 원래 Y 위치
    float shakeDuration;   // 남은 흔들림 시간
    float shakeMagnitude;  // 흔들림 강도

public:
    ObjectShake();
    ~ObjectShake() override;


    void Trigger(float duration, float magnitude);

    void Start(GraphicsContext* gfx) override;
    void Input() override;
    void Update(float dt) override;
    void Render(GraphicsContext* gfx) override;
};
