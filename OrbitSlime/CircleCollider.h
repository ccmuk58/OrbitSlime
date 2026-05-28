#pragma once
#include "Object.h"
#include "AsteroidMovement.h"

// 원형 충돌을 감지하는 컴포넌트
class CircleCollider : public Component
{
private:
    GameObject* target = nullptr; // 충돌을 검사할 대상 (슬라임)
    float myBaseRadius;           // 내 원본 메쉬 반지름
    float targetBaseRadius;       // 대상의 원본 메쉬 반지름
    AsteroidMovement* movement;

public:
    CircleCollider(GameObject* target, float myBaseRadius, float targetBaseRadius, AsteroidMovement* movement);
    ~CircleCollider() override;

    void Start(GraphicsContext* gfx) override;
    void Input() override;
    void Update(float dt) override;
    void Render(GraphicsContext* gfx) override;
};