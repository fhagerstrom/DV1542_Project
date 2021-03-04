#pragma once

#include "DXInit.h"
#include "DirectXMath.h"

class ProjectEngine : public DXInit
{
private:


public:
	ProjectEngine(HINSTANCE hInstance);
	~ProjectEngine();

	// Overridden setup and loop methods, which will be called automatically
	/*void Init();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);*/
};

