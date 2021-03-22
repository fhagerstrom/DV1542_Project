#pragma once
#pragma once
#include "DXInit.h"
#include <DirectXMath.h>

namespace dx = DirectX;


struct PointLight
{
	dx::XMFLOAT4 col;
	dx::XMFLOAT3 pos;

	void SetPLValues(dx::XMFLOAT4 colour, dx::XMFLOAT3 position)
	{
		col = colour;
		pos = position;
	}
};

struct SpotLight
{
	float specPower;
	dx::XMFLOAT3 dir;

	void SetSLValues(float power, dx::XMFLOAT3 direction)
	{
		specPower = power;
		dir = direction;
	}
};