#include "DigitTextureDisplay.h"

#include <vector>

namespace
{
    const int FONT_COLUMNS = 16;
    const int FONT_ROWS = 8;

    bool GetAtlasCell(char ch, int& column, int& row)
    {
        unsigned char code = (unsigned char)ch;

        if (code <= 0x7F)
        {
            column = code % FONT_COLUMNS;
            row = code / FONT_COLUMNS;
            return true;
        }

        return false;
    }

    float GetCharacterWidth(char ch, float digitWidth)
    {
        if (ch == ' ')
        {
            return digitWidth * 0.55f;
        }

        if (ch == ':' || ch == '.' || ch == '\'' || ch == ',')
        {
            return digitWidth * 0.45f;
        }

        if (ch == 'i' || ch == 'j' || ch == 'l' || ch == 'r' || ch == 't')
        {
            return digitWidth * 0.62f;
        }

        if (ch == 'I')
        {
            return digitWidth * 0.70f;
        }

        return digitWidth;
    }
}

DigitTextureDisplay::DigitTextureDisplay(Mesh* mesh, float width, float height, float gap)
    : targetMesh(mesh), digitWidth(width), digitHeight(height), spacing(gap)
{
}

DigitTextureDisplay::~DigitTextureDisplay()
{
}

void DigitTextureDisplay::SetDisplayText(const std::string& text)
{
    initialText = text;

    if (cachedGfx)
    {
        SetText(text);
    }
}

void DigitTextureDisplay::Start(GraphicsContext* gfx)
{
    cachedGfx = gfx;
    SetText(initialText);
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
    if (!targetMesh || !cachedGfx || text == currentText)
    {
        return;
    }

    currentText = text;

    std::vector<Vertex> vertices;
    const float cellU = 1.0f / (float)FONT_COLUMNS;
    const float cellV = 1.0f / (float)FONT_ROWS;
    const float glyphLeftInset = 0.00f;
    const float glyphRightInset = 0.38f;
    const float glyphTopInset = 0.04f;
    const float glyphBottomInset = 0.02f;
    float totalWidth = 0.0f;

    for (char ch : text)
    {
        if (ch == ' ' || (ch >= 0x20 && ch <= 0x7F))
        {
            if (totalWidth > 0.0f)
            {
                totalWidth += spacing;
            }

            totalWidth += GetCharacterWidth(ch, digitWidth);
        }
    }

    if (totalWidth <= 0.0f)
    {
        totalWidth = digitWidth;
    }

    float x = -totalWidth * 0.5f;

    for (char ch : text)
    {
        float charWidth = GetCharacterWidth(ch, digitWidth);

        if (ch == ' ')
        {
            x += charWidth + spacing;
            continue;
        }

        int column = 0;
        int row = 0;
        if (!GetAtlasCell(ch, column, row))
        {
            continue;
        }

        float u0 = column * cellU + cellU * glyphLeftInset;
        float u1 = (column + 1) * cellU - cellU * glyphRightInset;
        float v0 = row * cellV + cellV * glyphTopInset;
        float v1 = (row + 1) * cellV - cellV * glyphBottomInset;

        float x0 = x;
        float x1 = x + charWidth;
        float y0 = -digitHeight * 0.5f;
        float y1 = digitHeight * 0.5f;
        XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
        XMFLOAT3 normal = { 0.0f, 0.0f, -1.0f };

        vertices.push_back({ { x0, y1, 0.0f }, color, normal, { u0, v0 } });
        vertices.push_back({ { x1, y1, 0.0f }, color, normal, { u1, v0 } });
        vertices.push_back({ { x0, y0, 0.0f }, color, normal, { u0, v1 } });

        vertices.push_back({ { x0, y0, 0.0f }, color, normal, { u0, v1 } });
        vertices.push_back({ { x1, y1, 0.0f }, color, normal, { u1, v0 } });
        vertices.push_back({ { x1, y0, 0.0f }, color, normal, { u1, v1 } });

        x += charWidth + spacing;
    }

    if (vertices.empty())
    {
        return;
    }

    targetMesh->Create(cachedGfx, vertices);
}
