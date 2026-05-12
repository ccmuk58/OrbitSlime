#pragma once

#include <chrono>

class DeltaTime
{
private:
    std::chrono::high_resolution_clock::time_point prevTime;

public:
    DeltaTime();

    float GetDelta();
};