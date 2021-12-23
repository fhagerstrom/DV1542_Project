#pragma once
#include "DirectXHelper.h"
// This is directional light

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
	XMMATRIX GetMatrix() const;// Called upon in shaderdata::Update()
};