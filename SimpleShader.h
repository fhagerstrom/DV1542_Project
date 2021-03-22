#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#include <unordered_map>
#include <vector>
#include <string>

// Store information about specific values in constant buffer.
struct SimpleShaderVar
{
	unsigned int offset;
	unsigned int size;
	unsigned int constantBufferIndex;
};

// Get information about specific constant buffer in a shader.
// Also for the local buffer.
struct SimpleConstantBuffer
{
	unsigned int size;
	unsigned int BindIndex;
	unsigned char* LocalDataBuffer;

	std::string name;
	std::vector<SimpleShaderVar> variables;
	ID3D11Buffer* constantBuffer;
};

// Information for a single SRV in a shader.
struct SimpleSRV
{
	unsigned int index; // Raw index.
	unsigned int bindIndex; // SRV register.
};

// Information for a single sampler in a shader
struct SimpleSampler
{
	unsigned int index; // Raw index.
	unsigned int bindIndex; // Sampler register.
};

// Abstract class for shader handling
class SimpleShaderHandling
{
protected:
	bool isShaderValid;

	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	ID3DBlob* shaderBlob;

	// Resource count
	unsigned int constantBufferCount;

	// Maps for variables and buffers
	SimpleConstantBuffer* constantBuffers; // For index-based lookup
	std::vector<SimpleSRV*>	shaderResourceViews;
	std::vector<SimpleSampler*>	samplerStates;
	std::unordered_map<std::string, SimpleConstantBuffer*> cbTable;
	std::unordered_map<std::string, SimpleShaderVar> varTable;
	std::unordered_map<std::string, SimpleSRV*> textureTable;
	std::unordered_map<std::string, SimpleSampler*> samplerTable;

	// Pure virtual functions for dealing with shader types
	virtual bool CreateShader(ID3DBlob* shaderBlob) = 0;
	virtual void SetShaderAndCBs() = 0;

	virtual void CleanUp();

	// Helpers for finding data by name
	SimpleShaderVar* FindVariable(std::string name, int size);
	SimpleConstantBuffer* FindConstantBuffer(std::string name);

public:
	SimpleShaderHandling(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual ~SimpleShaderHandling();


};