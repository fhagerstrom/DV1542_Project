#pragma once
#include "Particle.h"
#include <d3d11.h>
#include <d3dcompiler.h>

#define ALIGN16 __declspec(align(16))

// PARTICLES - COMPUTE SHADER
ALIGN16
struct CBConstantParticles
{
	UINT maxParticles;
};

ALIGN16
struct CBDynamicParticles
{
	float deltaTime;
	XMFLOAT3 padding;
};

// PARTICLES - GEOMETRY SHADERS
ALIGN16
struct CBView
{
	XMMATRIX view;
	XMFLOAT3 cameraPosition;
	// float time;
};

ALIGN16
struct CBConstantResource
{
	float quadLength;
	XMMATRIX projection;
};


class ParticleShaders
{
private:
	int maxParticles;
	int quadLength;

	ID3D11VertexShader* particleVS;
	ID3D11PixelShader* particlePS;
	ID3D11GeometryShader* particleGS;
	ID3D11ComputeShader* particleCS;

	ID3DBlob* vertexShaderBlob;
	ID3DBlob* pixelShaderBlob;
	ID3DBlob* computeShaderBlob;
	ID3DBlob* geometryShaderBlob;
	ID3DBlob* errorBlob;

	HRESULT hr;

	ID3D11Buffer* particleBuffer;
	ID3D11Buffer* constantVariablesBuffer;
	ID3D11Buffer* dynamicVariablesBuffer;
	ID3D11Buffer* viewMatrixBuffer;
	ID3D11Buffer* constantResourcesBuffer;

	ID3D11ShaderResourceView* particleSRV;
	ID3D11UnorderedAccessView* particleUAV;
	ID3D11SamplerState* particleSampler;

	bool CreateInputBuffers(ID3D11Device* device, Particle* particles, int maxParticles);
	bool CreateComputeCBuffers(ID3D11Device* device, int maxParticles);

	void Shutdown();

public:
	ParticleShaders();
	~ParticleShaders();

	bool InitializeParticleShaders(ID3D11Device* device, Particle* particles, int maxParticles, float quadLength, XMMATRIX projection);
	bool CreateParticleSamplerState(ID3D11Device* device);
	bool SetShaders(ID3D11DeviceContext* context);
	bool CreateParticleCBuffers(ID3D11Device* device, float quadLength, XMMATRIX projection);
	void UpdateComputeShader(ID3D11DeviceContext* context);
	void UpdateGeometryShader(ID3D11DeviceContext* context, const XMMATRIX& view, const XMFLOAT3& cameraPosition);
	void UpdateComputeCBuffers(ID3D11DeviceContext* context, float deltaTime);
};