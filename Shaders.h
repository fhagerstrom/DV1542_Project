#pragma once

#define ALIGN16 __declspec(align(16))

#include <d3dcompiler.h>

#include "Model.h"
#include "Light.h"
#include "Camera.h"

using namespace DirectX;

static const int MAX_LIGHTS = 8;

ALIGN16
struct CBPerObject
{
	XMMATRIX wvp;
	XMMATRIX world;
	/*XMMATRIX view;
	XMMATRIX projection;*/
};

ALIGN16
struct CBLight
{
	XMFLOAT4 ambientLight;
	XMFLOAT4 diffuseLight;
	XMFLOAT4 specularLight;
	XMFLOAT3 lightPosition;
	float lightRange;

	XMFLOAT3 attenuation;
	float padding;
};

ALIGN16
struct CBMaterial
{
	XMFLOAT4 diffuseMtl;
	XMFLOAT4 ambientMtl;
	XMFLOAT4 specularMtl;
	XMFLOAT4 reflectionMtl;

	float shine;
	int hasTexture;
	int hasReflection;
	int padding;

	XMFLOAT4 translation;
};

ALIGN16
struct CBDeferred
{
	XMFLOAT4 lightColor;
	XMFLOAT3 lightPosition[MAX_LIGHTS];
	XMFLOAT3 cameraPosition;
	int nrOfLights;
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
	void ShutdownShader();

	bool SetCBuffers(ID3D11DeviceContext* context, Model* model, DirectX::XMMATRIX view, DirectX::XMMATRIX projection, Camera* camera, Light* light);
	void NewRenderShader(ID3D11DeviceContext* context, int indexCount, ID3D11SamplerState* sampler);

	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11ComputeShader* computeShader;
	ID3D11InputLayout* defaultLayout;

	// ID3D11 buffer declarations
	ID3D11Buffer* perObjectBuffer;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* cameraBuffer;
	ID3D11Buffer* materialBuffer;
	ID3D11Buffer* deferredBuffer;

	// Currently not in use, leaving them uncommented for now.
	ID3D11Buffer* computeInputBuffer;
	ID3D11Buffer* computeOutputBuffer;

	// Struct buffer declarations
	CBPerObject objectCb;
	CBCamera cameraCb;
	CBLight lightCb;
	CBMaterial materialCb;
	CBDeferred deferredCb;

	// Textures / Resources
	ID3D11SamplerState* samplerState;
	ID3D11ShaderResourceView* textureSRV;

	// Compute shader related resources
	ID3D11ShaderResourceView* computeInputSRV;
	ID3D11UnorderedAccessView* outputUAV;

	HRESULT hr;
	ID3DBlob* vertexShaderBlob;
	ID3DBlob* pixelShaderBlob;
	ID3DBlob* computeShaderBlob;
	ID3DBlob* errorBlob;

	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC cameraBufferDesc;
	D3D11_BUFFER_DESC deferredBufferDesc;

	// DESC is not in use at the moment, leaving them uncommented for now.
	D3D11_BUFFER_DESC inputDataDesc;
	D3D11_BUFFER_DESC outputDataDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	//_______________________________________________________________________________

	D3D11_SAMPLER_DESC samplerDesc;

public:
	Shader();
	~Shader();

	bool InitializeShaders(ID3D11Device* device, HWND hwnd, LPCWSTR vsFileName, LPCWSTR psFileName, LPCSTR VSEntry, LPCSTR PSEntry);
	bool CreateDefaultInputLayout(ID3D11Device* device);

	void Shutdown();
	bool NewRender(ID3D11DeviceContext* context, Model* model, XMMATRIX view, XMMATRIX projection, Camera* camera, Light* light, ID3D11SamplerState* sampler);
};
