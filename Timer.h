#pragma once
#include <Windows.h>

class Timer
{
private:
	__int64 baseTime;
	__int64 prevTime;
	__int64 currentTime;

	double frequency;
	double deltaTime;

public:
	Timer();
	~Timer();
	
	float DeltaTime() const;

	void Reset();  // Reset values before message loop.
	void Frame();  // Call every frame.

};