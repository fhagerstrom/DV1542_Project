#pragma once
#include "DxHandler.h"

//directional light
class Lighting
{
private:
	XMMATRIX matrix = XMMatrixIdentity();
public:
	struct Data
	{
		XMFLOAT4 color{ 1, 1, 1, 1 };
		XMFLOAT3 direction;
		float range;
	}data;

	Lighting() = default;
	Lighting(float range);
	~Lighting();
	XMMATRIX GetMatrix() const;// used in Shaders::Update()
};