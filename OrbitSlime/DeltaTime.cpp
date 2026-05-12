#include "DeltaTime.h"

DeltaTime::DeltaTime()
{
    prevTime = std::chrono::high_resolution_clock::now();
}

float DeltaTime::GetDelta()
{
    auto currTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> duration = currTime - prevTime;
    float dt = duration.count();
    prevTime = currTime;

    return dt;
}