#include "TimerDisplay.h"

#include <cstdio>
#include <string>

TimerDisplay::TimerDisplay(Mesh* mesh, float width, float height, float gap)
    : BitmapTextRenderer(mesh, width, height, gap)
{
}

void TimerDisplay::Reset()
{
    
    elapsedSeconds = 0.0f;
    lastShownDecisecond = -1;
    SetText("00:00.0");
}

void TimerDisplay::SetRunning(bool running)
{
    
    isRunning = running;
}

void TimerDisplay::Update(float dt)
{
    
    if (isRunning)
    {
        elapsedSeconds += dt;
    }

    int shownDecisecond = (int)(elapsedSeconds * 10.0f);

    
    if (shownDecisecond != lastShownDecisecond)
    {
        lastShownDecisecond = shownDecisecond;

        int minutes = shownDecisecond / 600;
        int seconds = (shownDecisecond / 10) % 60;
        int deciseconds = shownDecisecond % 10;

        char text[16];
        sprintf_s(text, "%02d:%02d.%01d", minutes, seconds, deciseconds);
        SetText(text);
    }
}
