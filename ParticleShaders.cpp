#include "ParticleShaders.h"

ParticleShaders::ParticleShaders()
{
}

ParticleShaders::~ParticleShaders()
{
}


bool ParticleShaders::InitializeParticleShaders(ID3D11Device* device, Particle* particles, int maxParticles, float quadLength, XMMATRIX projection)
{
	LPCWSTR vsFile = L"particleVS.hlsl";
	LPCWSTR psFile = L"particlePS.hlsl";
	LPCWSTR gsFile = L"particleGS.hlsl";
	LPCWSTR csFile = L"particleCS.hlsl";

	UINT shaderCompileFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;

	// Compile vertex shader
	hr = D3DCompileFromFile(vsFile, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0", shaderCompileFlags, NULL, &vertexShaderBlob, &errorBlob);
	if (FAILED(hr))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			MessageBox(NULL, vsFile, L"MISSING SHADER FILE!", MB_OK);
			errorBlob->Release();
		}

		vertexShaderBlob->Release();
		assert(false);
	}

	// Compile pixel shader
	hr = D3DCompileFromFile(psFile, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_5_0", shaderCompileFlags, NULL, &pixelShaderBlob, &errorBlob);
	if (FAILED(hr))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			MessageBox(NULL, psFile, L"MISSING SHADER FILE!", MB_OK);
			errorBlob->Release();
		}

		pixelShaderBlob->Release();
		assert(false);
	}

	// Compile compute shader
	hr = D3DCompileFromFile(csFile, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "cs_5_0", shaderCompileFlags, NULL, &computeShaderBlob, &errorBlob);
	if (FAILED(hr))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			MessageBox(NULL, csFile, L"MISSING SHADER FILE!", MB_OK);
			errorBlob->Release();
		}

		computeShaderBlob->Release();
		assert(false);
	}

	// Compile geometry shader
	hr = D3DCompileFromFile(gsFile, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "gs_5_0", shaderCompileFlags, NULL, &geometryShaderBlob, &errorBlob);
	if (FAILED(hr))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			MessageBox(NULL, csFile, L"MISSING SHADER FILE!", MB_OK);
			errorBlob->Release();
		}

		geometryShaderBlob->Release();
		assert(false);
	}

	// Create vertex shader
	hr = device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), NULL, &this->particleVS);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Couldn't create particle vertex shader!", L"SHADER ERROR", MB_OK);
		return false;
	}

	// Create pixel shader
	hr = device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), NULL, &this->particlePS);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Couldn't create particle pixel shader!", L"SHADER ERROR", MB_OK);
		return false;
	}

	// Create compute shader
	hr = device->CreateComputeShader(computeShaderBlob->GetBufferPointer(), computeShaderBlob->GetBufferSize(), NULL, &this->particleCS);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Couldn't create particle compute shader!", L"SHADER ERROR", MB_OK);
		return false;
	}

	// Create geometry shader
	hr = device->CreateGeometryShader(geometryShaderBlob->GetBufferPointer(), geometryShaderBlob->GetBufferSize(), NULL, &this->particleGS);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Couldn't create particle geometry shader!", L"SHADER ERROR", MB_OK);
		return false;
	}

	CreateParticleSamplerState(device);
	CreateInputBuffers(device, particles, maxParticles);
	CreateParticleCBuffers(device, quadLength, projection);
	CreateComputeCBuffers(device, maxParticles);

	return true;
}

bool ParticleShaders::CreateParticleSamplerState(ID3D11Device* device)
{
	D3D11_SAMPLER_DESC colorMapDesc;
	ZeroMemory(&colorMapDesc, sizeof(colorMapDesc));

	colorMapDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	colorMapDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	colorMapDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	colorMapDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	colorMapDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	colorMapDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = device->CreateSamplerState(&colorMapDesc, &particleSampler);

	if (FAILED(hr)) {
		MessageBox(NULL, L"Error creating sampler state!", L"Error", 0);
		return false;
	}
	return true;
}

bool ParticleShaders::CreateInputBuffers(ID3D11Device* device, Particle* particles, int maxParticles)
{
	D3D11_BUFFER_DESC buffDesc;
	ZeroMemory(&buffDesc, sizeof(buffDesc));
	buffDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	buffDesc.ByteWidth = sizeof(Particle) * maxParticles;
	buffDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	buffDesc.StructureByteStride = sizeof(Particle);
	buffDesc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = particles;

	device->CreateBuffer(&buffDesc, &data, &particleBuffer);
	if (FAILED(hr)) 
	{
		MessageBox(NULL, L"Could not create source particle buffer for compute shader!", L"Error", 0);
		return false;
	}

	ZeroMemory(&buffDesc, sizeof(buffDesc));
	particleBuffer->GetDesc(&buffDesc);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN; // Must be UNKOWN when creating structured Buffer
	srvDesc.Buffer.NumElements = maxParticles;

	hr = device->CreateShaderResourceView(particleBuffer, &srvDesc, &particleSRV);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Could not create source particle srv for compute shader!", L"Error", 0);
		return false;
	}

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(uavDesc));
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.Flags = 0;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Format = DXGI_FORMAT_UNKNOWN; //Must be UNKOWN when creating structured Buffer
	uavDesc.Buffer.NumElements = maxParticles;

	hr = device->CreateUnorderedAccessView(particleBuffer, &uavDesc, &particleUAV);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Could not create source particle buffer UAV for compute shaders!", L"Error", 0);
		return false;
	}

	return true;
}

bool ParticleShaders::SetShaders(ID3D11DeviceContext* context)
{
	context->GSSetConstantBuffers(0, 1, &viewMatrixBuffer);
	context->GSSetConstantBuffers(1, 1, &constantResourcesBuffer);

	context->IASetInputLayout(NULL);

	context->VSSetShader(particleVS, 0, 0);
	context->GSSetShader(particleGS, 0, 0);
	context->PSSetShader(particlePS, 0, 0);
	context->PSSetSamplers(0, 1, &particleSampler);

	return true;
}

bool ParticleShaders::CreateParticleCBuffers(ID3D11Device* device, float quadLength, XMMATRIX projection)
{
	D3D11_BUFFER_DESC cbBufferDesc;
	ZeroMemory(&cbBufferDesc, sizeof(cbBufferDesc));

	cbBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbBufferDesc.ByteWidth = sizeof(CBView);

	hr = device->CreateBuffer(&cbBufferDesc, 0, &viewMatrixBuffer);

	if (FAILED(hr)) 
	{
		MessageBox(NULL, L"Error creating constant view matrix buffer!", L"Error", 0);
		return false;
	}

	// Creating constant variables and matrices
	ZeroMemory(&cbBufferDesc, sizeof(cbBufferDesc));
	cbBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	cbBufferDesc.ByteWidth = sizeof(CBConstantResource);

	CBConstantResource data;
	data.quadLength = quadLength;
	data.projection = projection;

	D3D11_SUBRESOURCE_DATA cbData;
	ZeroMemory(&cbData, sizeof(cbData));
	cbData.pSysMem = &data;

	hr = device->CreateBuffer(&cbBufferDesc, &cbData, &constantResourcesBuffer);

	if (FAILED(hr)) 
	{
		MessageBox(NULL, L"Error creating constant resources buffer!", L"Error", 0);
		return false;
	}

	return true;
}

void ParticleShaders::UpdateComputeShader(ID3D11DeviceContext* context)
{
	context->CSSetShader(particleCS, NULL, 0);
	context->CSSetConstantBuffers(2, 1, &constantVariablesBuffer);
	context->CSSetConstantBuffers(3, 1, &dynamicVariablesBuffer);
	context->CSSetUnorderedAccessViews(0, 1, &particleUAV, NULL);
	context->Dispatch(32, 1, 1);

	ID3D11UnorderedAccessView* nullUAV = NULL;
	context->CSSetUnorderedAccessViews(0, 1, &nullUAV, NULL);

	context->VSSetShaderResources(0, 1, &particleSRV);
}

void ParticleShaders::UpdateGeometryShader(ID3D11DeviceContext* context, const XMMATRIX& view, const XMFLOAT3& cameraPosition)
{
	// Update the buffer which contains the viewMatrix and the camPosition
	CBView cbDataVMB;
	cbDataVMB.view = view;
	cbDataVMB.cameraPosition = cameraPosition;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(mappedResource));

	context->Map(viewMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	CBView* ptrVMB = static_cast<CBView*>(mappedResource.pData);
	memcpy(ptrVMB, &cbDataVMB, sizeof(CBView));

	context->Unmap(viewMatrixBuffer, 0);
}

void ParticleShaders::UpdateComputeCBuffers(ID3D11DeviceContext* devContext, float deltaTime)
{
	CBDynamicParticles cbData;
	ZeroMemory(&cbData, sizeof(cbData));
	cbData.deltaTime = deltaTime;

	D3D11_MAPPED_SUBRESOURCE data;
	ZeroMemory(&data, sizeof(data));

	devContext->Map(dynamicVariablesBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);

	CBDynamicParticles* ptr = static_cast<CBDynamicParticles*>(data.pData);

	memcpy(ptr, &cbData, sizeof(CBDynamicParticles));

	devContext->Unmap(dynamicVariablesBuffer, 0);
}

bool ParticleShaders::CreateComputeCBuffers(ID3D11Device* device, int maxParticles)
{
	// Create constant buffer for constant variables
	D3D11_BUFFER_DESC buffDesc;
	ZeroMemory(&buffDesc, sizeof(buffDesc));
	buffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffDesc.ByteWidth = sizeof(CBConstantParticles);
	buffDesc.Usage = D3D11_USAGE_DEFAULT;

	CBConstantParticles cbData;
	cbData.maxParticles = maxParticles;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = &cbData;

	hr = device->CreateBuffer(&buffDesc, &data, &constantVariablesBuffer);
	if (FAILED(hr)) 
	{
		MessageBox(NULL, L"Could not create constant buffer for constant variables for the compute shader!", L"Error", 0);
		return false;
	}

	// Create constant buffer for dynamic variables
	ZeroMemory(&buffDesc, sizeof(buffDesc));
	buffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffDesc.ByteWidth = sizeof(CBDynamicParticles);
	buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffDesc.Usage = D3D11_USAGE_DYNAMIC;

	hr = device->CreateBuffer(&buffDesc, 0, &dynamicVariablesBuffer);
	if (FAILED(hr)) 
	{
		MessageBox(NULL, L"Could not create constant buffer for dynamic variables for the compute shader!", L"Error", 0);
		return false;
	}

	return true;
}

void ParticleShaders::Shutdown()
{
}
