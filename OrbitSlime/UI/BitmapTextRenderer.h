#pragma once

#include "Object.h"
#include "Render.h"

#include <string>

class BitmapTextRenderer : public Component
{
protected:
    // 실제 화면에 그려질 글자 사각형들이 들어가는 메쉬.
    // 텍스트가 바뀌면 이 메쉬의 정점 UV를 새로 만들어서 교체한다.
    Mesh* targetMesh = nullptr;

    // Mesh::Create를 다시 호출하려면 DirectX 디바이스가 필요해서 Start 때 저장해 둔다.
    GraphicsContext* cachedGfx = nullptr;

    // 같은 텍스트를 매 프레임 다시 만들지 않기 위한 캐시.
    std::string currentText;
    std::string initialText = "00:00.0";

    // 월드 좌표 기준 글자 한 칸의 크기와 글자 사이 간격.
    float digitWidth = 0.08f;
    float digitHeight = 0.16f;
    float spacing = 0.012f;

    // 문자열을 글자별 사각형 메쉬로 바꾼다.
    void SetText(const std::string& text);

public:
    BitmapTextRenderer(Mesh* mesh, float width = 0.08f, float height = 0.16f, float gap = 0.012f);
    ~BitmapTextRenderer() override;

    void SetDisplayText(const std::string& text);

    void Start(GraphicsContext* gfx) override;
    void Input() override;
    void Update(float dt) override;
    void Render(GraphicsContext* gfx) override;
};
