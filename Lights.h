#pragma once
#include "DXInit.h"

#include <DirectXMath.h>

namespace dx = DirectX;


struct PointLight
{
	dx::XMFLOAT4 colour;
	dx::XMFLOAT3 position;

	void SetpointLightValues(dx::XMFLOAT4 _colour, dx::XMFLOAT3 _position)
	{
		colour = _colour;
		position = _position;
	}
};

struct SpotLight
{
	dx::XMFLOAT3 Direction;
	float SpotPower;
	

	void SetSpotLightValues(dx::XMFLOAT3 _direction, float _power)
	{
		Direction = _direction;
		SpotPower = _power;
	}
};
