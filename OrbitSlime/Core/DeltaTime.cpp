#include "DeltaTime.h"

DeltaTime::DeltaTime()
{
    prevTime = std::chrono::high_resolution_clock::now();
}

float DeltaTime::GetDelta()
{
    std::chrono::high_resolution_clock::time_point currTime = std::chrono::high_resolution_clock::now();
    float dt = std::chrono::duration<float>(currTime - prevTime).count();
    prevTime = currTime;
    return dt;
}
