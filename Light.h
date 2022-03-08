#pragma once
#include <DirectXMath.h>
using namespace DirectX;

class Light
{
private:
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT3 lightDirection;
	float specularPower;
	XMFLOAT4 specular;

public:
	Light() {};
	~Light() {};

	// TODO: ADD SET AND GET FUNCTIONS FOR SPECULAR STUFF
	void SetAmbientColor(float r, float g, float b, float a) { ambient = XMFLOAT4(r, g, b, a); };
	void SetDiffuseColor(float r, float g, float b, float a) { diffuse = XMFLOAT4(r, g, b, a); };
	void SetSpecularColor(float r, float g, float b, float a) { specular = XMFLOAT4(r, g, b, a); };
	void SetSpecularPower(float power) { specularPower = power; };
	void SetDirection(float x, float y, float z) { lightDirection = XMFLOAT3(x, y, z); };

	XMFLOAT4 GetAmbientColor() { return ambient; };
	XMFLOAT4 GetDiffuseColor() { return diffuse; };
	XMFLOAT4 GetSpecularColor() { return specular; };
	XMFLOAT3 GetDirection() { return lightDirection; };
	float GetSpecularPower() { return specularPower; };
};