#pragma once
#include <DirectXMath.h>

#include "DXInit.h"

namespace dx = DirectX;


struct PointLight
{
	dx::XMFLOAT4 Colour;
	dx::XMFLOAT3 Position;

	void SetpointLightValues(dx::XMFLOAT4 _colour, dx::XMFLOAT3 _position)
	{
		Colour = _colour;
		Position = _position;
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
