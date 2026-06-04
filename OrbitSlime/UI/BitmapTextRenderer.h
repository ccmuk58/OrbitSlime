#pragma once

#include "Object.h"
#include "Render.h"

#include <string>

class BitmapTextRenderer : public Component
{
protected:
    
    
    Mesh* targetMesh = nullptr;

    
    GraphicsContext* cachedGfx = nullptr;

    
    std::string currentText;
    std::string initialText = "00:00.0";

    
    float digitWidth = 0.08f;
    float digitHeight = 0.16f;
    float spacing = 0.012f;

    
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
