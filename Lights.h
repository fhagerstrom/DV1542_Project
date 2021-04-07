#pragma once
#include <DirectXMath.h>

#include "DXInit.h"

namespace dx = DirectX;


struct PointLight
{
	dx::XMFLOAT4 Color;
	dx::XMFLOAT3 Position;

	void SetpointLightValues(dx::XMFLOAT4 _color, dx::XMFLOAT3 _position)
	{
		Color = _color;
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
