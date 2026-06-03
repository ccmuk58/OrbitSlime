#pragma once
#include "Object.h"
#include "MeshGenerator.h" // Mesh가 정의된 헤더
#include "ScoreManager.h"
#include <string>
#include <vector>

class ScoreDisplay : public Component
{
private:
    Mesh* targetMesh;
    int lastScore;
    GraphicsContext* cachedGfx; // 메쉬를 재생성할 때 필요한 그래픽스 컨텍스트

public:
    ScoreDisplay(Mesh* mesh);
    ~ScoreDisplay() override;

    void Start(GraphicsContext* gfx) override;
    void Input() override;
    void Update(float dt) override;
    void Render(GraphicsContext* gfx) override;
};
