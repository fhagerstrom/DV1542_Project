#include "Timer.h"

Timer::Timer()
{
	frequency = 0.0;
	deltaTime = -1.0f;
	baseTime = 0;
	prevTime = 0;
	currentTime = 0;

	__int64 countsPerSecond;

	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSecond);
	frequency = 1.0 / (double)countsPerSecond;
}

Timer::~Timer()
{

}

float Timer::DeltaTime() const
{
	return (float)deltaTime;
}

void Timer::Reset()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

	baseTime = currentTime;
	prevTime = currentTime;
}

void Timer::Frame()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

	// Time difference between this frame and the previous.
	deltaTime = (currentTime - prevTime) * frequency;

	// Current time is the previous time for next frame
	prevTime = currentTime;
}



