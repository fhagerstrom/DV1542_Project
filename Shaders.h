#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>

using namespace DirectX;

struct MatrixBuffer
{
	XMMATRIX world;
	XMMATRIX view;
	XMMATRIX projection;
};

struct LightBuffer
{
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT3 lightDirection;
	float specularPower;
	XMFLOAT4 specular;
};

struct CameraBuffer
{
	XMFLOAT3 cameraPos;
	float padding;
};

class Shader
{
private:
	bool InitializeShader(ID3D11Device*, HWND, LPCWSTR, LPCWSTR);
	void ShutdownShader();

	bool SetShaderParameters(ID3D11DeviceContext* context, XMMATRIX world, XMMATRIX view, XMMATRIX projection, ID3D11ShaderResourceView* textureSrv, 
		XMFLOAT3 lightDirection, XMFLOAT4 ambient, XMFLOAT4 diffuse, XMFLOAT3 cameraPos, XMFLOAT4 specular, float specularPower);
	void RenderShader(ID3D11DeviceContext*, int);

	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* defaultLayout;
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* cameraBuffer;
	ID3D11SamplerState* samplerState;

public:
	Shader();
	~Shader();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext* context, int indexCount, XMMATRIX world, XMMATRIX view, XMMATRIX projection, ID3D11ShaderResourceView* textureSrv, 
		XMFLOAT3 lightDirection, XMFLOAT4 ambient, XMFLOAT4 diffuse, XMFLOAT3 cameraPos, XMFLOAT4 specular, float specularPower);
};
