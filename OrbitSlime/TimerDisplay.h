#pragma once

#include "DigitTextureDisplay.h"

class TimerDisplay : public DigitTextureDisplay
{
private:
    float elapsedSeconds = 0.0f;
    int lastShownDecisecond = -1;
    bool isRunning = false;

public:
    TimerDisplay(Mesh* mesh, float width = 0.08f, float height = 0.16f, float gap = 0.012f);

    void Reset();
    void SetRunning(bool running);
    void Update(float dt) override;
};
