#pragma once
#include <chrono>

struct Timer
{
private:
    std::chrono::time_point<std::chrono::steady_clock> startTime;

public:
    void Start()
    {
        startTime = std::chrono::steady_clock::now();
    }

    float DeltaTime()
    {
        auto stop = std::chrono::steady_clock::now();
        return std::chrono::duration_cast<std::chrono::microseconds>(stop - startTime).count() / 1000000.0f;
    }
};