#include "Frame.h"

#include <cstdio>

Frame::Frame(float printInterval)
    : printIntervalSeconds(printInterval)
{
}

void Frame::Update(float dt)
{
    if (!enabled)
    {
        return;
    }

    frameCount++;
    elapsedSeconds += dt;

    if (elapsedSeconds >= printIntervalSeconds)
    {
        int fps = frameCount / elapsedSeconds;
        printf("FPS: %d\n", fps);

        frameCount = 0;
        elapsedSeconds = 0.0f;
    }
}

void Frame::SetEnabled(bool value)
{
    enabled = value;
}

bool Frame::IsEnabled() const
{
    return enabled;
}
