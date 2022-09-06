#pragma once

#define ALIGN16 __declspec(align(16))

#include <d3dcompiler.h>

#include "Model.h"
#include "Light.h"
#include "Camera.h"

using namespace DirectX;

ALIGN16
struct CBPerObject
{
	XMMATRIX world;
	XMMATRIX view;
	XMMATRIX projection;
	XMMATRIX wvp;
};

ALIGN16
struct CBLight
{
	XMFLOAT4 ambientLight;
	XMFLOAT4 diffuseLight;
	XMFLOAT4 specularLight;
	XMFLOAT3 lightDirection;
	float specularPower;
};

ALIGN16
struct CBMaterial
{
	XMFLOAT4 ambientMtl;
	XMFLOAT4 diffuseMtl;
	XMFLOAT4 specularMtl;
	XMFLOAT4 reflectionColor;

	float shine;
	int hasTexture;
	int hasReflection;
	int padding;

	XMFLOAT4 translation;
};

ALIGN16
struct CBCamera
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

	bool SetCBuffers(ID3D11DeviceContext* context, Model* model, DirectX::XMMATRIX view, DirectX::XMMATRIX projection, Camera* camera, Light* light);
	void RenderShader(ID3D11DeviceContext* context, int indexCount);
	void NewRenderShader(ID3D11DeviceContext* context, int indexCount, ID3D11SamplerState* sampler);

	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* defaultLayout;

	// ID3D11 buffer declarations
	ID3D11Buffer* perObjectBuffer;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* cameraBuffer;
	ID3D11Buffer* materialBuffer;

	// Struct buffer declarations
	CBPerObject objectCb;
	CBCamera cameraCb;
	CBLight lightCb;
	CBMaterial materialCb;

	// Textures / Resources
	ID3D11SamplerState* samplerState;
	ID3D11ShaderResourceView* textureSRV;
	ID3D11ShaderResourceView* normalMapSRV;

	HRESULT hr;
	ID3DBlob* vertexShaderBlob;
	ID3DBlob* pixelShaderBlob;
	ID3DBlob* errorBlob;

	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC cameraBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;

public:
	Shader();
	~Shader();

	bool Initialize(ID3D11Device*, HWND);
	bool InitializeShaders(ID3D11Device* device, HWND hwnd, LPCWSTR vsFileName, LPCWSTR psFileName, LPCSTR VSEntry, LPCSTR PSEntry);

	bool CreateDefaultInputLayout(ID3D11Device* device);

	void Shutdown();
	bool Render(ID3D11DeviceContext* context, int indexCount, XMMATRIX world, XMMATRIX view, XMMATRIX projection, ID3D11ShaderResourceView* textureSrv, 
		XMFLOAT3 lightDirection, XMFLOAT4 ambient, XMFLOAT4 diffuse, XMFLOAT3 cameraPos, XMFLOAT4 specular, float specularPower);
	bool NewRender(ID3D11DeviceContext* context, Model* model, XMMATRIX view, XMMATRIX projection, Camera* camera, Light* light, ID3D11SamplerState* sampler);
};
