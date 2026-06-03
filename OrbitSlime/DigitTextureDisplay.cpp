#include "DigitTextureDisplay.h"

#include <vector>

DigitTextureDisplay::DigitTextureDisplay(Mesh* mesh, float width, float height, float gap)
    : targetMesh(mesh), digitWidth(width), digitHeight(height), spacing(gap)
{
}

DigitTextureDisplay::~DigitTextureDisplay()
{
}

void DigitTextureDisplay::Start(GraphicsContext* gfx)
{
    // Start 이후부터는 SetText에서 메쉬를 만들 수 있다.
    cachedGfx = gfx;
    SetText("0");
}

void DigitTextureDisplay::Input()
{
}

void DigitTextureDisplay::Update(float dt)
{
}

void DigitTextureDisplay::Render(GraphicsContext* gfx)
{
}

void DigitTextureDisplay::SetText(const std::string& text)
{
    // 메쉬/그래픽 컨텍스트가 없거나 값이 그대로면 새 정점을 만들 필요가 없다.
    if (!targetMesh || !cachedGfx || text == currentText)
    {
        return;
    }

    currentText = text;

    std::vector<Vertex> vertices;
    int visibleCount = 0;

    // 가운데 정렬하려면 실제로 그릴 숫자 개수가 먼저 필요하다.
    for (char ch : text)
    {
        if (ch >= '0' && ch <= '9')
        {
            visibleCount++;
        }
    }

    const float slotWidth = digitWidth + spacing;
    const float totalWidth = visibleCount > 0
        ? visibleCount * digitWidth + (visibleCount - 1) * spacing
        : digitWidth;

    // 전체 숫자 폭의 절반만큼 왼쪽에서 시작하면 오브젝트 중심 기준으로 정렬된다.
    float x = -totalWidth * 0.5f;

    for (char ch : text)
    {
        // 지금은 숫자만 지원한다. 나중에 ':' 같은 문자를 넣고 싶으면 여기에서 처리하면 된다.
        if (ch < '0' || ch > '9')
        {
            x += slotWidth;
            continue;
        }

        int digit = ch - '0';

        // numbers.png가 0~9를 가로로 같은 폭에 배치한 이미지라서
        // 숫자 하나의 UV 폭은 1 / 10 = 0.1 이다.
        float u0 = digit * 0.1f;
        float u1 = u0 + 0.1f;

        // 숫자 하나는 사각형 1개이고, DirectX에 넘기기 위해 삼각형 2개로 만든다.
        float x0 = x;
        float x1 = x + digitWidth;
        float y0 = -digitHeight * 0.5f;
        float y1 = digitHeight * 0.5f;
        XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
        XMFLOAT3 normal = { 0.0f, 0.0f, -1.0f };

        vertices.push_back({ { x0, y1, 0.0f }, color, normal, { u0, 0.0f } });
        vertices.push_back({ { x1, y1, 0.0f }, color, normal, { u1, 0.0f } });
        vertices.push_back({ { x0, y0, 0.0f }, color, normal, { u0, 1.0f } });

        vertices.push_back({ { x0, y0, 0.0f }, color, normal, { u0, 1.0f } });
        vertices.push_back({ { x1, y1, 0.0f }, color, normal, { u1, 0.0f } });
        vertices.push_back({ { x1, y0, 0.0f }, color, normal, { u1, 1.0f } });

        x += slotWidth;
    }

    if (vertices.empty())
    {
        // 빈 문자열이 들어와도 화면에는 최소 0을 보여준다.
        SetText("0");
        return;
    }

    // 새 정점 배열로 기존 vertex buffer를 교체한다.
    targetMesh->Create(cachedGfx, vertices);
}
