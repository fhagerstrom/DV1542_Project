#pragma once

#define ALIGN16 __declspec(align(16))

#include <d3dcompiler.h>

#include "Model.h"
#include "Light.h"
#include "Camera.h"
#include "DeferredRender.h"
#include "ParticleSystem.h"

using namespace DirectX;

static const int MAX_LIGHTS = 4;

ALIGN16
struct CBPerObject
{
	XMMATRIX wvp;
	XMMATRIX world;
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
	bool hasTexture;
	bool hasReflection;
	int padding;
};

ALIGN16
struct CBDeferredLight
{
	XMFLOAT4 ambientLight;
	XMFLOAT4 diffuseLight;
	XMFLOAT4 specularLight;
	XMFLOAT3 lightPosition;
	float lightRange;

	XMFLOAT3 attenuation;
	float padding1;

	XMFLOAT3 cameraPosition;
	float padding2;
	// int nrOfLights;

	XMMATRIX projection;
	XMMATRIX lightVP;
};

ALIGN16
struct CBCamera
{
	XMFLOAT3 cameraPos;
	float padding;
};

//// PARTICLES - COMPUTE SHADER
//ALIGN16
//struct CBConstantParticles
//{
//	UINT maxParticles;
//};
//
//ALIGN16
//struct CBDynamicParticles
//{
//	float deltaTime;
//	XMFLOAT3 padding;
//};
//
//// PARTICLES - GEOMETRY SHADERS
//ALIGN16
//struct CBView
//{
//	XMMATRIX view;
//	XMFLOAT3 cameraPosition;
//	float time;
//};
//
//ALIGN16
//struct CBConstantResource
//{
//	float quadLength;
//	XMMATRIX projection;
//};

class Shader
{
private:
	int windowWidth, windowHeight;

	void ShutdownShader();

	bool SetObjectCBuffers(ID3D11DeviceContext* context, Model* model, XMMATRIX view, XMMATRIX projection, Camera* camera, Light* light);
	bool CreateParticleCBuffers(ID3D11Device* device, int maxParticles);
	void RenderShader(ID3D11DeviceContext* context, int indexCount, ID3D11SamplerState* sampler);

	// REGULAR FORWARD SHADERS
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;

	// DEFERRED SHADERS
	ID3D11VertexShader* deferredVS;
	ID3D11PixelShader* deferredPS;
	ID3D11ComputeShader* deferredCS;

	// PARTICLE SHADERS
	ID3D11VertexShader* particleVS;
	ID3D11PixelShader* particlePS;
	ID3D11ComputeShader* particleCS;
	ID3D11GeometryShader* particleGS;
	
	// INPUT LAYOUT(S)
	ID3D11InputLayout* defaultLayout;

	DxHandler* dxHandler;
	DeferredRender* deferredPtr;

	// ID3D11 buffer declarations
	ID3D11Buffer* perObjectBuffer;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* cameraBuffer;
	ID3D11Buffer* materialBuffer;
	ID3D11Buffer* deferredBuffer;
	ID3D11Buffer* particleBuffer;

	// PARTICLES
	ID3D11Buffer* constantVariablesBuffer;
	ID3D11Buffer* dynamicVariablesBuffer;
	ID3D11Buffer* viewBuffer;
	ID3D11Buffer* constantResourceBuffer;

	// Struct buffer declarations
	CBPerObject objectCb;
	CBCamera cameraCb;
	CBLight lightCb;
	CBMaterial materialCb;
	CBDeferredLight deferredLightCb;

	// Compute shader / Geometry shader constant buffers for particles.
	/*CBConstantParticles constantParticleCb;
	CBDynamicParticles dynamicParticleCb;
	CBView viewMatrixCb;
	CBConstantResource constantResourceCb;*/

	// Textures / Resources
	ID3D11SamplerState* samplerState;
	ID3D11SamplerState* particleSamplerState;
	ID3D11ShaderResourceView* textureSRV;
	ID3D11ShaderResourceView* particleSrv;
	ID3D11UnorderedAccessView* particleUav;
	ID3D11RenderTargetView* particleRtv;
	ID3D11DepthStencilView* particleDsv;

	HRESULT hr;
	ID3DBlob* vertexShaderBlob;
	ID3DBlob* pixelShaderBlob;
	ID3DBlob* computeShaderBlob;
	ID3DBlob* geometryShaderBlob;
	ID3DBlob* errorBlob;

	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC cameraBufferDesc;
	D3D11_BUFFER_DESC deferredBufferDesc;

	// Declare texture, RTV and SRV arrays for the deferred buffers
	ID3D11Texture2D* renderTargetTextureArray[BUFFER_COUNT];
	ID3D11RenderTargetView* renderTargetViewArray[BUFFER_COUNT];
	ID3D11ShaderResourceView* shaderResourceViewArray[BUFFER_COUNT];

	// Declare null pointers for clearing shaders, CBuffers, SRVs and RTVs
	ID3D11VertexShader* nullVS = NULL;
	ID3D11PixelShader* nullPS = NULL;
	ID3D11Buffer* nullBuffer = NULL;
	ID3D11RenderTargetView* nullSingleRtv = NULL;
	ID3D11ShaderResourceView* nullSingleSrv = NULL;
	ID3D11UnorderedAccessView* nullUav = NULL;
	ID3D11RenderTargetView* nullRtvArray[BUFFER_COUNT] = { NULL };
	ID3D11ShaderResourceView* nullSrvArray[BUFFER_COUNT] = { NULL };

	D3D11_SAMPLER_DESC samplerDesc;

public:
	Shader();
	~Shader();

	bool InitializeShaders(ID3D11Device* device, HWND hwnd, LPCWSTR vsFileName, LPCWSTR psFileName, LPCSTR VSEntry, LPCSTR PSEntry);
	bool IntializeDeferredShaders(ID3D11Device* device, DxHandler* dxHandler, HWND hwnd, LPCWSTR vsFileName, LPCWSTR psFileName, LPCWSTR csFileName, LPCSTR VSEntry, LPCSTR PSEntry, LPCSTR CSEntry);
	bool InitializeParticleShaders(ID3D11Device* device, HWND hwnd, LPCWSTR vsFileName, LPCWSTR psFileName, LPCWSTR csFileName, LPCWSTR gsFileName, LPCSTR VSEntry, LPCSTR PSEntry, LPCSTR CSEntry, LPCSTR GSEntry);
	
	bool CreateDefaultInputLayout(ID3D11Device* device);

	void BindGeometryPass(ID3D11DeviceContext* context);
	void BindLightPass(ID3D11DeviceContext* context);
	void Shutdown();

	void UpdateGeometryParticleCBuffers(ID3D11DeviceContext* context, XMMATRIX projection, const XMFLOAT3& cameraPosition);
	void UpdateComputeParticleCBuffers(ID3D11DeviceContext* context, float deltaTime);
	void UpdateParticleShader(ID3D11DeviceContext* context, ParticleSystem* particleSystem);
	bool Render(ID3D11DeviceContext* context, Model* model, XMMATRIX view, XMMATRIX projection, Camera* camera, Light* light, ID3D11SamplerState* sampler);
};
