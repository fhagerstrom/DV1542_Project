#pragma once
#include <DirectXMath.h>
using namespace DirectX;

class Light
{
private:
	XMFLOAT4 ambientColor;
	XMFLOAT4 diffuseColor;
	XMFLOAT4 specularColor;
	XMFLOAT3 lightPosition;
	float lightRange;

	XMFLOAT3 attenuation;

public:
	Light() {};
	~Light() {};

	// Setter functions
	void SetAmbientColor(float r, float g, float b, float a) { ambientColor = XMFLOAT4(r, g, b, a); }
	void SetDiffuseColor(float r, float g, float b, float a) { diffuseColor = XMFLOAT4(r, g, b, a); }
	void SetSpecularColor(float r, float g, float b, float a) { specularColor = XMFLOAT4(r, g, b, a); }
	void SetLightPosition(float x, float y, float z) { lightPosition = XMFLOAT3(x, y, z); };
	void SetLightRange(float range) { lightRange = range; }
	void SetAttenuation(float c1, float c2, float c3) { attenuation = XMFLOAT3(c1, c2, c3); }

	// Getter functions
	XMFLOAT4 GetAmbientColor() { return this->ambientColor; }
	XMFLOAT4 GetDiffuseColor() { return this->diffuseColor; }
	XMFLOAT4 GetSpecularColor() { return this->specularColor; }
	XMFLOAT3 GetLightPosition() { return this->lightPosition; }
	float GetLightRange() { return this->lightRange; };
	XMFLOAT3 GetAttenuation() { return this->attenuation; }
};

struct DirectionalLight
{

};

struct PointLight
{

};