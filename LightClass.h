#pragma once
#include <DirectXMath.h>


namespace dx = DirectX;

class LightClass
{
public:
	LightClass();
	LightClass(const LightClass&);
	~LightClass();

	void SetDiffuseColor(float, float, float, float);
	void SetDirection(float, float, float);

	dx::XMFLOAT4 GetDiffuseColor();
	dx::XMFLOAT3 GetDirection();

private:
	
	dx::XMFLOAT4 diffuseColor;
	dx::XMFLOAT3 direction;
};