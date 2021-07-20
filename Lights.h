#pragma once
#include "DXInit.h"

#include <DirectXMath.h>

namespace dx = DirectX;


struct PointLight
{
	dx::XMFLOAT4 colour;
	dx::XMFLOAT3 position;

	void SetpointLightValues(dx::XMFLOAT4 colour, dx::XMFLOAT3 position)
	{
		this->colour = colour;
		this->position = position;
	}
};

struct SpotLight
{
	dx::XMFLOAT3 direction;
	float spotPower;
	

	void SetSpotLightValues(dx::XMFLOAT3 direction, float power)
	{
		this->direction = direction;
		spotPower = power;
	}
};
