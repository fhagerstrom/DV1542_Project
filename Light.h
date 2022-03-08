#pragma once
#include <DirectXMath.h>
using namespace DirectX;

class Light
{
private:
	XMFLOAT4 ambientColor;
	XMFLOAT4 diffuseColor;
	XMFLOAT3 lightDirection;
	float specularPower;
	XMFLOAT4 specularColor;

public:
	Light() {};
	~Light() {};

	// TODO: ADD SET AND GET FUNCTIONS FOR SPECULAR STUFF
	void SetAmbientColor(float r, float g, float b, float a) { ambientColor = XMFLOAT4(r, g, b, a); };
	void SetDiffuseColor(float r, float g, float b, float a) { diffuseColor = XMFLOAT4(r, g, b, a); };
	void SetSpecularColor(float r, float g, float b, float a) { specularColor = XMFLOAT4(r, g, b, a); };
	void SetSpecularPower(float power) { specularPower = power; };
	void SetDirection(float x, float y, float z) { lightDirection = XMFLOAT3(x, y, z); };

	XMFLOAT4 GetAmbientColor() { return ambientColor; };
	XMFLOAT4 GetDiffuseColor() { return diffuseColor; };
	XMFLOAT4 GetSpecularColor() { return specularColor; };
	XMFLOAT3 GetDirection() { return lightDirection; };
	float GetSpecularPower() { return specularPower; };
};