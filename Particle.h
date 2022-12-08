#pragma once
#include <DirectXMath.h>

using namespace DirectX;

struct Particle
{
	XMFLOAT3 currentPosition;
	XMFLOAT3 oldPosition;

	Particle(float x, float y, float z) 
	{
		currentPosition = XMFLOAT3(x, y, z);
		oldPosition = XMFLOAT3(x, y, z);
	}

	Particle()
	{
		currentPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
		oldPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
	}

	XMFLOAT3 SetPosition(XMFLOAT3& position) { currentPosition = position; }
	XMFLOAT3 GetPosition() { return currentPosition; }
};