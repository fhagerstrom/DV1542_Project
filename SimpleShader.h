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
	unsigned int byteOffset;
	unsigned int size;
	unsigned int constantBufferIndex;
};

// Get information about specific constant buffer in a shader.
// Also for the local buffer.
struct SimpleConstantBuffer
{
	unsigned int size;
	unsigned int BindIndex;
	unsigned char* localDataBuffer;

	std::string Name;
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
class SimpleShaderHandler
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
	SimpleShaderHandler(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual ~SimpleShaderHandler();

	bool LoadShaderFiles(LPCWSTR shaderFile);
	bool ShaderValid()
	{
		return isShaderValid;
	}

	void SetShader();
	void CopyAllBufferData();
	void CopyBufferData(unsigned int index);
	void CopyBufferData(std::string bufferName);

	bool SetData(std::string name, const void* data, unsigned int size);

	bool SetInt(std::string name, int data);
	bool SetFloat(std::string name, float data);
	bool SetFloat2(std::string name, const float data[2]);
	bool SetFloat2(std::string name, const DirectX::XMFLOAT2 data);
	bool SetFloat3(std::string name, const float data[3]);
	bool SetFloat3(std::string name, const DirectX::XMFLOAT3 data);
	bool SetFloat4(std::string name, const float data[4]);
	bool SetFloat4(std::string name, const DirectX::XMFLOAT4 data);
	bool SetMatrix4x4(std::string name, const float data[16]);
	bool SetMatrix4x4(std::string name, const DirectX::XMFLOAT4X4 data);

	// Set shader resources
	virtual bool SetShaderResourceView(std::string name, ID3D11ShaderResourceView* srv) = 0;
	virtual bool SetSamplerState(std::string name, ID3D11SamplerState* samplerState) = 0;

	// Get data about variables and resources
	const SimpleShaderVar* GetVariableInfo(std::string name);

	const SimpleSRV* GetShaderResourceViewInfo(std::string name);
	const SimpleSRV* GetShaderResourceViewInfo(unsigned int index);
	unsigned int GetShaderResourceViewCount() { return textureTable.size(); }

	const SimpleSampler* GetSamplerInfo(std::string name);
	const SimpleSampler* GetSamplerInfo(unsigned int index);
	unsigned int GetSamplerCount() { return samplerTable.size(); }

	// Get data about constant buffers
	unsigned int GetBufferCount();
	unsigned int GetBufferSize(unsigned int index);
	const SimpleConstantBuffer* GetBufferInfo(std::string name);
	const SimpleConstantBuffer* GetBufferInfo(unsigned int index);

	// Misc getters
	ID3DBlob* GetShaderBlob() { return shaderBlob; }
};

// Derived Vertex Shader class
class SimpleVertexShader : public SimpleShaderHandler
{
protected:
	bool perInstanceCompatible;
	ID3D11InputLayout* inputLayout;
	ID3D11VertexShader* shader;
	bool CreateShader(ID3DBlob* shaderBlob);
	void SetShaderAndCBs();
	void CleanUp();

public:
	SimpleVertexShader(ID3D11Device* device, ID3D11DeviceContext* context);
	SimpleVertexShader(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11InputLayout* inputLayout, bool perInstanceCompatible);
	~SimpleVertexShader();

	ID3D11VertexShader* GetDirectXShader() 
	{ 
		return shader; 
	}

	ID3D11InputLayout* GetInputLayout() 
	{ 
		return inputLayout; 
	}
	
	bool GetPerInstanceCompatible() 
	{ 
		return perInstanceCompatible; 
	}

	bool SetShaderResourceView(std::string name, ID3D11ShaderResourceView* srv);
	bool SetSamplerState(std::string name, ID3D11SamplerState* samplerState);
};

// Derived pixel shader class
class SimplePixelShader : public SimpleShaderHandler
{
protected:
	bool perInstanceCompatible;
	ID3D11PixelShader* shader;
	bool CreateShader(ID3DBlob* shaderBlob);
	void SetShaderAndCBs();
	void CleanUp();

public:
	SimplePixelShader(ID3D11Device* device, ID3D11DeviceContext* context);
	~SimplePixelShader();
	ID3D11PixelShader* GetDirectXShader() 
	{ 
		return shader; 
	}

	bool SetShaderResourceView(std::string name, ID3D11ShaderResourceView* srv);
	bool SetSamplerState(std::string name, ID3D11SamplerState* samplerState);
};