#include "lightclass.h"


LightClass::LightClass()
{
}


LightClass::LightClass(const LightClass& other)
{
}


LightClass::~LightClass()
{
}


void LightClass::SetDiffuseColor(float red, float green, float blue, float alpha)
{
	diffuseColor = dx::XMFLOAT4(red, green, blue, alpha);
	return;
}


void LightClass::SetDirection(float x, float y, float z)
{
	direction = dx::XMFLOAT3(x, y, z);
	return;
}


dx::XMFLOAT4 LightClass::GetDiffuseColor()
{
	return diffuseColor;
}


dx::XMFLOAT3 LightClass::GetDirection()
{
	return direction;
}