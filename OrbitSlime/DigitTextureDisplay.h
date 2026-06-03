#pragma once

#include "Object.h"
#include "Render.h"

#include <string>

class DigitTextureDisplay : public Component
{
protected:
    // 실제 화면에 그려질 숫자 사각형들이 들어가는 메쉬.
    // 텍스트가 바뀌면 이 메쉬의 정점 UV를 새로 만들어서 교체한다.
    Mesh* targetMesh = nullptr;

    // Mesh::Create를 다시 호출하려면 DirectX 디바이스가 필요해서 Start 때 저장해 둔다.
    GraphicsContext* cachedGfx = nullptr;

    // 같은 숫자를 매 프레임 다시 만들지 않기 위한 캐시.
    std::string currentText;
    std::string initialText = "00:00.0";

    // 월드 좌표 기준 숫자 한 칸의 크기와 숫자 사이 간격.
    float digitWidth = 0.08f;
    float digitHeight = 0.16f;
    float spacing = 0.012f;

    // "123" 같은 문자열을 숫자별 사각형 메쉬로 바꾼다.
    void SetText(const std::string& text);

public:
    DigitTextureDisplay(Mesh* mesh, float width = 0.08f, float height = 0.16f, float gap = 0.012f);
    ~DigitTextureDisplay() override;

    void SetDisplayText(const std::string& text);

    void Start(GraphicsContext* gfx) override;
    void Input() override;
    void Update(float dt) override;
    void Render(GraphicsContext* gfx) override;
};
