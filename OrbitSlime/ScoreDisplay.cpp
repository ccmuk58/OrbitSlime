/*#include "ScoreDisplay.h"
#include "MeshGenerator.h"

ScoreDisplay::ScoreDisplay(Mesh* mesh) : targetMesh(mesh), lastScore(-1), cachedGfx(nullptr)
{
}

ScoreDisplay::~ScoreDisplay()
{
}

void ScoreDisplay::Start(GraphicsContext* gfx)
{
    // Update에서 메쉬를 다시 만들려면 gfx가 필요하므로 저장해둠!
    cachedGfx = gfx;
}

void ScoreDisplay::Input() {}

void ScoreDisplay::Update(float dt)
{
    // 슬라임의 피격 횟수나, 행성의 점수 등 원하는 점수를 가져옴
    int currentScore = ScoreManager::slimeHitCount;

    // 점수가 바뀌었을 때만 메쉬를 새로 깎아서 업데이트!
    if (currentScore != lastScore && cachedGfx != nullptr)
    {
        lastScore = currentScore;
        std::string text = std::to_string(currentScore);

        float charWidth = 0.1f;
        float charHeight = 0.3f;
        float uvWidth = 0.1f;

        // ★ 가운데 정렬 핵심 공식!
        // 글자 개수만큼 전체 너비를 구한 뒤, 그 절반만큼 왼쪽(-)으로 당겨서 시작해.
        float totalWidth = text.length() * charWidth;
        float startX = -(totalWidth / 2.0f);

        std::vector<Vertex> vText;

        for (size_t i = 0; i < text.length(); ++i)
        {
            int num = text[i] - '0';
            if (num < 0 || num > 9) continue;

            float uStart = (float)num * uvWidth;
            float uEnd = uStart + uvWidth;

            // startX를 기준으로 글자들을 오른쪽으로 나열
            float xOffset = startX + (float)i * charWidth;

            // 1. 좌상 (Top-Left)
            vText.push_back(Vertex{
                XMFLOAT3(xOffset + 0.0f, charHeight, 0.0f), // pos
                XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),           // col
                XMFLOAT3(0.0f, 0.0f, -1.0f),                // normal
                XMFLOAT2(uStart, 0.0f)                      // uv
                });

            // 2. 우상 (Top-Right)
            vText.push_back(Vertex{
                XMFLOAT3(xOffset + charWidth, charHeight, 0.0f),
                XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
                XMFLOAT3(0.0f, 0.0f, -1.0f),
                XMFLOAT2(uEnd, 0.0f)
                });

            // 3. 좌하 (Bottom-Left)
            vText.push_back(Vertex{
                XMFLOAT3(xOffset + 0.0f, 0.0f, 0.0f),
                XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
                XMFLOAT3(0.0f, 0.0f, -1.0f),
                XMFLOAT2(uStart, 1.0f)
                });

            // 4. 우하 (Bottom-Right)
            vText.push_back(Vertex{
                XMFLOAT3(xOffset + charWidth, 0.0f, 0.0f),
                XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
                XMFLOAT3(0.0f, 0.0f, -1.0f),
                XMFLOAT2(uEnd, 1.0f)
                });

            // 5. 좌하 (Bottom-Left)
            vText.push_back(Vertex{
                XMFLOAT3(xOffset + 0.0f, 0.0f, 0.0f),
                XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
                XMFLOAT3(0.0f, 0.0f, -1.0f),
                XMFLOAT2(uStart, 1.0f)
                });

            // 6. 우상 (Top-Right)
            vText.push_back(Vertex{
                XMFLOAT3(xOffset + charWidth, charHeight, 0.0f),
                XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
                XMFLOAT3(0.0f, 0.0f, -1.0f),
                XMFLOAT2(uEnd, 0.0f)
                });
        }


        targetMesh->Create(cachedGfx, vText);
    }
}

void ScoreDisplay::Render(GraphicsContext* gfx) {}*/