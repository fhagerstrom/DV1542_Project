#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <fstream>
using namespace DirectX;

class LightingShader
{
private:
	struct MatrixBuffer
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX proj;
		XMMATRIX lightView;
		XMMATRIX lightProj;
	};

	struct LightBuffer
	{
		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;
		XMFLOAT3 lightDirection;// = XMFLOAT3(0.0, -1.0, 0.0);
		float specularPower;
		XMFLOAT4 specular;
	};

	struct LightBuffer2
	{
		XMFLOAT3 lightPosition;
		float padding;
	};

public:
	LightingShader();
	LightingShader(const LightingShader&);
	~LightingShader();
	bool Initialize(ID3D11Device* device, HWND hwnd);
	void Shutdown();
	bool Render(ID3D11DeviceContext* context, int indexCount, XMMATRIX world, XMMATRIX view, XMMATRIX proj, ID3D11ShaderResourceView** textureArray,
		XMFLOAT3 directionLight, XMFLOAT4, XMFLOAT4, XMFLOAT4, float, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT3);
}