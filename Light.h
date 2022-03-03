#pragma once
#include <DirectXMath.h>
using namespace DirectX;

class Light
{
private:
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT3 lightDirection;

public:
	Light() {};
	~Light() {};

	void SetAmbientColor(float r, float g, float b, float a) { ambient = XMFLOAT4(r, g, b, a); };
	void SetDiffuseColor(float r, float g, float b, float a) { diffuse = XMFLOAT4(r, g, b, a); };
	void SetDirection(float x, float y, float z) { lightDirection = XMFLOAT3(x, y, z); };

	XMFLOAT4 GetAmbientColor() { return ambient; };
	XMFLOAT4 GetDiffuseColor() { return diffuse; };
	XMFLOAT3 GetDirection() { return lightDirection; };
};