#pragma once
#include <DirectXMath.h>

using namespace DirectX;

class Frustum
{
public:
	Frustum();
	~Frustum();

	void ConstructFrustum(float screenDepth, XMMATRIX view, XMMATRIX projection);

	bool CheckCube(float xCenter, float yCenter, float zCenter, float radius);

private:
	XMFLOAT4 planes[6]; //or XMVECTOR, depending on what works. 
};