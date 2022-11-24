#include "Shaders.h"

Shader::Shader()
{
	vertexShader = 0;
	pixelShader = 0;
	defaultLayout = 0;
	perObjectBuffer = 0;
	lightBuffer = 0;
	samplerState = 0;
}

Shader::~Shader()
{
}

void Shader::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects.
	ShutdownShader();

	return;
}

bool Shader::Render(ID3D11DeviceContext* context, Model* model, XMMATRIX view, XMMATRIX projection, Camera* camera, Light* light, ID3D11SamplerState* sampler)
{
	bool result;
	result = SetObjectCBuffers(context, model, view, projection, camera, light);
	if (!result) 
		return false;

	RenderShader(context, model->GetIndexCount(), sampler);

	return true;
}

bool Shader::RenderParticles(ID3D11DeviceContext* context, ParticleSystem* particleSystem, int maxParticles, XMMATRIX projection, float quadLength)
{
	bool result;
	result = SetConstantParticleCBuffers(context, particleSystem, maxParticles, projection, quadLength);
	if (!result)
	{
		MessageBox(NULL, L"Couldn't set particle constant buffers!", L"SHADER ERROR", MB_OK);
		return false;
	}

	SetParticleShaders(context, particleSystem);

	return true;
}

bool Shader::SetParticleShaders(ID3D11DeviceContext* context, ParticleSystem* particleSystem)
{

	particleSrv = particleSystem->GetSrv();

	context->GSSetConstantBuffers(0, 1, &viewBuffer);
	context->GSSetConstantBuffers(1, 1, &constantResourceBuffer);

	context->IASetInputLayout(NULL);

	context->VSSetShader(particleVS, 0, 0);
	context->GSSetShader(particleGS, 0, 0);
	context->PSSetShader(particlePS, 0, 0);
	context->PSSetSamplers(0, 1, &particleSamplerState);
	// context->PSSetShaderResources(0, 1, &particleSrv);

	return true;
}

bool Shader::IntializeDeferredShaders(ID3D11Device* device, DxHandler* dxHandler, HWND hwnd, LPCWSTR vsFileName, LPCWSTR psFileName, LPCWSTR csFileName, LPCSTR VSEntry, LPCSTR PSEntry, LPCSTR CSEntry)
{
	deferredPtr = new DeferredRender;

	// Compile vertex shader
	hr = D3DCompileFromFile(vsFileName, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, VSEntry, "vs_5_0", D3DCOMPILE_ENABLE_STRICTNESS, NULL, &vertexShaderBlob, &errorBlob);
	if (FAILED(hr))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			MessageBox(hwnd, vsFileName, L"MISSING SHADER FILE!", MB_OK);
			errorBlob->Release();
		}

		vertexShaderBlob->Release();
		assert(false);
	}

	// Compile pixel shader
	hr = D3DCompileFromFile(psFileName, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, PSEntry, "ps_5_0", D3DCOMPILE_ENABLE_STRICTNESS, NULL, &pixelShaderBlob, &errorBlob);
	if (FAILED(hr))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			MessageBox(hwnd, psFileName, L"MISSING SHADER FILE!", MB_OK);
			errorBlob->Release();
		}

		pixelShaderBlob->Release();
		assert(false);
	}

	UINT csFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;


	// Compile compute shader
	hr = D3DCompileFromFile(csFileName, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, CSEntry, "cs_5_0", csFlags, NULL, &computeShaderBlob, &errorBlob);
	if (FAILED(hr))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			MessageBox(hwnd, csFileName, L"MISSING SHADER FILE!", MB_OK);
			errorBlob->Release();
		}

		computeShaderBlob->Release();
		assert(false);
	}

	// Create vertex shader
	hr = device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), NULL, &this->deferredVS);
	if (FAILED(hr))
	{
		MessageBox(hwnd, L"Couldn't create deferred vertex shader!", L"SHADER ERROR", MB_OK);
		return false;
	}

	// Create pixel shader
	hr = device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), NULL, &this->deferredPS);
	if (FAILED(hr))
	{
		MessageBox(hwnd, L"Couldn't create deferred pixel shader!", L"SHADER ERROR", MB_OK);
		return false;
	}

	// Create compute shader
	hr = device->CreateComputeShader(computeShaderBlob->GetBufferPointer(), computeShaderBlob->GetBufferSize(), NULL, &this->deferredCS);
	if (FAILED(hr))
	{
		MessageBox(hwnd, L"Couldn't create compute shader!", L"SHADER ERROR", MB_OK);
		return false;
	}

	/* CONSTANT BUFFER DESCRIPTIONS */

	D3D11_BUFFER_DESC cBufferDescription;

	// Setup the description of the bufferPerObject constant buffer that is in the vertex shader.
	ZeroMemory(&cBufferDescription, sizeof(D3D11_BUFFER_DESC));
	cBufferDescription.Usage = D3D11_USAGE_DEFAULT;
	cBufferDescription.ByteWidth = static_cast<uint32_t>(sizeof(CBPerObject) + (16 - (sizeof(CBPerObject) % 16)));
	cBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cBufferDescription.CPUAccessFlags = 0;
	cBufferDescription.MiscFlags = 0;
	cBufferDescription.StructureByteStride = 0;

	hr = device->CreateBuffer(&cBufferDescription, NULL, &perObjectBuffer);
	if (FAILED(hr))
	{
		return false;
	}

	// Setup the description of the light constant buffer that is in the vertex shader.
	ZeroMemory(&cBufferDescription, sizeof(cBufferDescription));
	cBufferDescription.Usage = D3D11_USAGE_DEFAULT;
	cBufferDescription.ByteWidth = static_cast<uint32_t>(sizeof(CBLight) + (16 - (sizeof(CBLight) % 16)));
	cBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cBufferDescription.CPUAccessFlags = 0;
	cBufferDescription.MiscFlags = 0;
	cBufferDescription.StructureByteStride = 0;

	hr = device->CreateBuffer(&cBufferDescription, NULL, &lightBuffer);
	if (FAILED(hr))
	{
		return false;
	}

	// Setup description for the deferred lighting buffer in the compute shader.
	ZeroMemory(&cBufferDescription, sizeof(cBufferDescription));
	cBufferDescription.Usage = D3D11_USAGE_DEFAULT;
	cBufferDescription.ByteWidth = static_cast<uint32_t>(sizeof(CBDeferredLight) + (16 - (sizeof(CBDeferredLight) % 16)));
	cBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cBufferDescription.CPUAccessFlags = 0;
	cBufferDescription.MiscFlags = 0;
	cBufferDescription.StructureByteStride = 0;

	hr = device->CreateBuffer(&cBufferDescription, NULL, &deferredBuffer);
	if (FAILED(hr))
	{
		return false;
	}

	// Setup the description of the camera constant buffer that is in the vertex shader.
	ZeroMemory(&cBufferDescription, sizeof(cBufferDescription));
	cBufferDescription.Usage = D3D11_USAGE_DEFAULT;
	cBufferDescription.ByteWidth = static_cast<uint32_t>(sizeof(CBCamera) + (16 - (sizeof(CBCamera) % 16)));
	cBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cBufferDescription.CPUAccessFlags = 0;
	cBufferDescription.MiscFlags = 0;
	cBufferDescription.StructureByteStride = 0;

	hr = device->CreateBuffer(&cBufferDescription, NULL, &cameraBuffer);
	if (FAILED(hr))
	{
		return false;
	}

	// Setup the description of the material constant buffer that is in the pixel shader.
	ZeroMemory(&cBufferDescription, sizeof(cBufferDescription));
	cBufferDescription.Usage = D3D11_USAGE_DEFAULT;
	cBufferDescription.ByteWidth = static_cast<uint32_t>(sizeof(CBMaterial) + (16 - (sizeof(CBMaterial) % 16)));
	cBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cBufferDescription.CPUAccessFlags = 0;
	cBufferDescription.MiscFlags = 0;
	cBufferDescription.StructureByteStride = 0;

	hr = device->CreateBuffer(&cBufferDescription, NULL, &materialBuffer);
	if (FAILED(hr))
	{
		return false;
	}

	// Initialize deferred descriptions from DeferredRender class
	// Get back buffer pointer as well as creating the UAV
	windowWidth = 1280;
	windowHeight = 720;
	deferredPtr->Initialize(device, dxHandler, windowWidth, windowHeight);

	// DENNA KLAGAR PÅ STATIONÄRA, VENNE VARFÖR FÖR ATT DET FUNKAR PÅ LAPTOPEN
	// dxHandler->GetBackBuffer()->Release();

	return true;
}

bool Shader::InitializeParticleShaders(ID3D11Device* device, HWND hwnd, LPCWSTR vsFileName, LPCWSTR psFileName, LPCWSTR csFileName, LPCWSTR gsFileName, LPCSTR VSEntry, LPCSTR PSEntry, LPCSTR CSEntry, LPCSTR GSEntry)
{
	UINT shaderCompileFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;

	// Compile vertex shader
	hr = D3DCompileFromFile(vsFileName, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, VSEntry, "vs_5_0", shaderCompileFlags, NULL, &vertexShaderBlob, &errorBlob);
	if (FAILED(hr))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			MessageBox(hwnd, vsFileName, L"MISSING SHADER FILE!", MB_OK);
			errorBlob->Release();
		}

		vertexShaderBlob->Release();
		assert(false);
	}

	// Compile pixel shader
	hr = D3DCompileFromFile(psFileName, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, PSEntry, "ps_5_0", shaderCompileFlags, NULL, &pixelShaderBlob, &errorBlob);
	if (FAILED(hr))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			MessageBox(hwnd, psFileName, L"MISSING SHADER FILE!", MB_OK);
			errorBlob->Release();
		}

		pixelShaderBlob->Release();
		assert(false);
	}

	// Compile compute shader
	hr = D3DCompileFromFile(csFileName, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, CSEntry, "cs_5_0", shaderCompileFlags, NULL, &computeShaderBlob, &errorBlob);
	if (FAILED(hr))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			MessageBox(hwnd, csFileName, L"MISSING SHADER FILE!", MB_OK);
			errorBlob->Release();
		}

		computeShaderBlob->Release();
		assert(false);
	}

	// Compile geometry shader
	hr = D3DCompileFromFile(gsFileName, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, GSEntry, "gs_5_0", shaderCompileFlags, NULL, &geometryShaderBlob, &errorBlob);
	if (FAILED(hr))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			MessageBox(hwnd, csFileName, L"MISSING SHADER FILE!", MB_OK);
			errorBlob->Release();
		}

		geometryShaderBlob->Release();
		assert(false);
	}

	// Create vertex shader
	hr = device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), NULL, &this->particleVS);
	if (FAILED(hr))
	{
		MessageBox(hwnd, L"Couldn't create particle vertex shader!", L"SHADER ERROR", MB_OK);
		return false;
	}

	// Create pixel shader
	hr = device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), NULL, &this->particlePS);
	if (FAILED(hr))
	{
		MessageBox(hwnd, L"Couldn't create particle pixel shader!", L"SHADER ERROR", MB_OK);
		return false;
	}

	// Create compute shader
	hr = device->CreateComputeShader(computeShaderBlob->GetBufferPointer(), computeShaderBlob->GetBufferSize(), NULL, &this->particleCS);
	if (FAILED(hr))
	{
		MessageBox(hwnd, L"Couldn't create particle compute shader!", L"SHADER ERROR", MB_OK);
		return false;
	}

	// Create geometry shader
	hr = device->CreateGeometryShader(geometryShaderBlob->GetBufferPointer(), geometryShaderBlob->GetBufferSize(), NULL, &this->particleGS);
	if (FAILED(hr))
	{
		MessageBox(hwnd, L"Couldn't create particle geometry shader!", L"SHADER ERROR", MB_OK);
		return false;
	}

	// SAMPLER STATE FOR PARTICLES
	
	D3D11_SAMPLER_DESC particleSamplerDesc;
	ZeroMemory(&particleSamplerDesc, sizeof(particleSamplerDesc));

	particleSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	particleSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	particleSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	particleSamplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	particleSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	particleSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = device->CreateSamplerState(&particleSamplerDesc, &particleSamplerState);

	if (FAILED(hr)) 
	{
		MessageBox(NULL, L"Error creating sampler state!", L"SHADER ERROR", 0);
		return false;
	}

	/* CONSTANT BUFFER DESCRIPTIONS */

	D3D11_BUFFER_DESC cBufferDescription;

	// Setup the description of the constant particle constant buffer that is in the compute shader.
	ZeroMemory(&cBufferDescription, sizeof(D3D11_BUFFER_DESC));
	cBufferDescription.Usage = D3D11_USAGE_DEFAULT;
	cBufferDescription.ByteWidth = static_cast<uint32_t>(sizeof(CBConstantParticles) + (16 - (sizeof(CBConstantParticles) % 16)));
	cBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cBufferDescription.CPUAccessFlags = 0;
	cBufferDescription.MiscFlags = 0;
	cBufferDescription.StructureByteStride = 0;

	hr = device->CreateBuffer(&cBufferDescription, NULL, &constantVariablesBuffer);
	if (FAILED(hr))
	{
		return false;
	}

	// Setup the description of the dynamic particle constant buffer that is in the compute shader.
	ZeroMemory(&cBufferDescription, sizeof(D3D11_BUFFER_DESC));
	cBufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	cBufferDescription.ByteWidth = static_cast<uint32_t>(sizeof(CBDynamicParticles) + (16 - (sizeof(CBDynamicParticles) % 16)));
	cBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cBufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cBufferDescription.MiscFlags = 0;
	cBufferDescription.StructureByteStride = 0;

	hr = device->CreateBuffer(&cBufferDescription, NULL, &dynamicVariablesBuffer);
	if (FAILED(hr))
	{
		return false;
	}

	// Setup the description of the constant particle constant buffer that is in the geometry shader.
	ZeroMemory(&cBufferDescription, sizeof(D3D11_BUFFER_DESC));
	cBufferDescription.Usage = D3D11_USAGE_DEFAULT;
	cBufferDescription.ByteWidth = static_cast<uint32_t>(sizeof(CBConstantResource) + (16 - (sizeof(CBConstantResource) % 16)));
	cBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cBufferDescription.CPUAccessFlags = 0;
	cBufferDescription.MiscFlags = 0;
	cBufferDescription.StructureByteStride = 0;

	hr = device->CreateBuffer(&cBufferDescription, NULL, &constantResourceBuffer);
	if (FAILED(hr))
	{
		return false;
	}

	// Setup the description of the dynamic particle constant buffer that is in the geometry shader.
	ZeroMemory(&cBufferDescription, sizeof(D3D11_BUFFER_DESC));
	cBufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	cBufferDescription.ByteWidth = static_cast<uint32_t>(sizeof(CBView) + (16 - (sizeof(CBView) % 16)));
	cBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cBufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cBufferDescription.MiscFlags = 0;
	cBufferDescription.StructureByteStride = 0;

	hr = device->CreateBuffer(&cBufferDescription, NULL, &viewBuffer);
	if (FAILED(hr))
	{
		return false;
	}

	vertexShaderBlob->Release();
	pixelShaderBlob->Release();
	computeShaderBlob->Release();
	geometryShaderBlob->Release();

	return true;
}

bool Shader::InitializeShaders(ID3D11Device* device, HWND hwnd, LPCWSTR vsFileName, LPCWSTR psFileName, LPCSTR VSEntry, LPCSTR PSEntry)
{
	/* COMPILE AND CREATE VERTEX AND PIXEL SHADERS */

	// Compile vertex shader
	hr = D3DCompileFromFile(vsFileName, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, VSEntry, "vs_5_0", D3DCOMPILE_ENABLE_STRICTNESS, NULL, &vertexShaderBlob, &errorBlob);
	if (FAILED(hr))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			MessageBox(hwnd, vsFileName, L"MISSING SHADER FILE!", MB_OK);
			errorBlob->Release();
		}

		vertexShaderBlob->Release();
		assert(false);
	}

	// Compile pixel shader
	hr = D3DCompileFromFile(psFileName, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, PSEntry, "ps_5_0", D3DCOMPILE_ENABLE_STRICTNESS, NULL, &pixelShaderBlob, &errorBlob);
	if (FAILED(hr))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			MessageBox(hwnd, psFileName, L"MISSING SHADER FILE!", MB_OK);
			errorBlob->Release();
		}

		pixelShaderBlob->Release();
		assert(false);
	}

	// Create vertex shader
	hr = device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), NULL, &this->vertexShader);
	if (FAILED(hr))
	{
		MessageBox(hwnd, L"Couldn't create vertex shader", L"SHADER ERROR!", MB_OK);
		return false;
	}

	// Create pixel shader
	hr = device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), NULL, &this->pixelShader);
	if (FAILED(hr))
	{
		MessageBox(hwnd, L"Couldn't create pixel shader", L"SHADER ERROR!", MB_OK);
		return false;
	}

	/* CONSTANT BUFFER DESCRIPTIONS */

	D3D11_BUFFER_DESC cBufferDescription;

	// Setup the description of the bufferPerObject constant buffer that is in the vertex shader.
	ZeroMemory(&cBufferDescription, sizeof(D3D11_BUFFER_DESC));
	cBufferDescription.Usage = D3D11_USAGE_DEFAULT;
	cBufferDescription.ByteWidth = static_cast<uint32_t>(sizeof(CBPerObject) + (16 - (sizeof(CBPerObject) % 16)));
	cBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cBufferDescription.CPUAccessFlags = 0;
	cBufferDescription.MiscFlags = 0;
	cBufferDescription.StructureByteStride = 0;

	hr = device->CreateBuffer(&cBufferDescription, NULL, &perObjectBuffer);
	if (FAILED(hr))
	{
		return false;
	}

	// Setup the description of the camera constant buffer that is in the vertex shader.
	ZeroMemory(&cBufferDescription, sizeof(cBufferDescription));
	cBufferDescription.Usage = D3D11_USAGE_DEFAULT;
	cBufferDescription.ByteWidth = static_cast<uint32_t>(sizeof(CBCamera) + (16 - (sizeof(CBCamera) % 16)));
	cBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cBufferDescription.CPUAccessFlags = 0;
	cBufferDescription.MiscFlags = 0;
	cBufferDescription.StructureByteStride = 0;

	hr = device->CreateBuffer(&cBufferDescription, NULL, &cameraBuffer);
	if (FAILED(hr))
	{
		return false;
	}

	// Setup the description of the light constant buffer that is in the vertex shader.
	ZeroMemory(&cBufferDescription, sizeof(cBufferDescription));
	cBufferDescription.Usage = D3D11_USAGE_DEFAULT;
	cBufferDescription.ByteWidth = static_cast<uint32_t>(sizeof(CBLight) + (16 - (sizeof(CBLight) % 16)));
	cBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cBufferDescription.CPUAccessFlags = 0;
	cBufferDescription.MiscFlags = 0;
	cBufferDescription.StructureByteStride = 0;

	hr = device->CreateBuffer(&cBufferDescription, NULL, &lightBuffer);
	if (FAILED(hr))
	{
		return false;
	}

	// Setup the description of the material constant buffer that is in the pixel shader.
	ZeroMemory(&cBufferDescription, sizeof(cBufferDescription));
	cBufferDescription.Usage = D3D11_USAGE_DEFAULT;
	cBufferDescription.ByteWidth = static_cast<uint32_t>(sizeof(CBMaterial) + (16 - (sizeof(CBMaterial) % 16)));
	cBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cBufferDescription.CPUAccessFlags = 0;
	cBufferDescription.MiscFlags = 0;
	cBufferDescription.StructureByteStride = 0;

	hr = device->CreateBuffer(&cBufferDescription, NULL, &materialBuffer);
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

void Shader::ShutdownShader()
{
	// Release the sampler state.
	if (samplerState)
	{
		samplerState->Release();
		samplerState = 0;
	}

	// Release the matrix constant buffer.
	if(perObjectBuffer)
	{
		perObjectBuffer->Release();
		perObjectBuffer = 0;
	}

	// Release the camera constant buffer.
	if (cameraBuffer)
	{
		cameraBuffer->Release();
		cameraBuffer = 0;
	}

	if (lightBuffer)
	{
		lightBuffer->Release();
		lightBuffer = 0;
	}

	if (materialBuffer)
	{
		materialBuffer->Release();
		materialBuffer = 0;
	}

	// Release the layout.
	if(defaultLayout)
	{
		defaultLayout->Release();
		defaultLayout = 0;
	}

	// Release the pixel shader.
	if(pixelShader)
	{
		pixelShader->Release();
		pixelShader = 0;
	}

	// Release the vertex shader.
	if(vertexShader)
	{
		vertexShader->Release();
		vertexShader = 0;
	}

	// Release the compute shader.
	if (deferredCS)
	{
		deferredCS->Release();
		deferredCS = 0;
	}

	return;
}

bool Shader::CreateDefaultInputLayout(ID3D11Device* device)
{
	/* CREATE INPUT LAYOUT DESCRIPTION */

	D3D11_INPUT_ELEMENT_DESC inputLayoutPosTexNormal[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,	0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	hr = device->CreateInputLayout(inputLayoutPosTexNormal, ARRAYSIZE(inputLayoutPosTexNormal), vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &defaultLayout);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Couldn't create input layout!", L"SHADER ERROR", MB_OK);
		return false;
	}

	vertexShaderBlob->Release();

	return true;
}

void Shader::BindGeometryPass(ID3D11DeviceContext* context)
{
	deferredPtr->SetRenderTargets(context);
	context->ClearDepthStencilView(deferredPtr->depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0.0f);

	float color[4] = { 0.1f, 0.0f, 0.3f, 1.0f };

	deferredPtr->ClearRenderTargets(context, color[0], color[1], color[2], color[3]);
}

void Shader::BindLightPass(ID3D11DeviceContext* context)
{
	float color[4] = { 0.1f, 0.0f, 0.3f, 1.0f };

	context->OMSetRenderTargets(BUFFER_COUNT, nullRtvArray, NULL);

	context->CSSetShader(deferredCS, NULL, 0);

	context->CSSetShaderResources(0, BUFFER_COUNT, deferredPtr->shaderResourceViewArray);
	context->CSSetUnorderedAccessViews(0, 1, &deferredPtr->deferredUav, NULL);

	context->Dispatch(60, 128, 1);

	// NULL resources and buffers
	context->CSSetShaderResources(0, BUFFER_COUNT, nullSrvArray);
	context->CSSetConstantBuffers(0, NULL, &nullBuffer);
	context->CSSetUnorderedAccessViews(0, 1, &nullUav, NULL);
}

bool Shader::SetObjectCBuffers(ID3D11DeviceContext* context, Model* model, DirectX::XMMATRIX view, DirectX::XMMATRIX projection, Camera* camera, Light* light)
{
	// OBJECT / MATRIX BUFFERS
	XMMATRIX worldViewProjection;
	worldViewProjection = model->GetWorldMatrix() * view * projection;

	objectCb.wvp = XMMatrixTranspose(worldViewProjection);
	objectCb.world = DirectX::XMMatrixTranspose(model->GetWorldMatrix());

	context->UpdateSubresource(perObjectBuffer, 0, nullptr, &objectCb, 0, 0);
	context->VSSetConstantBuffers(0, 1, &perObjectBuffer);

	// Set shader texture resource in the pixel shader	
	if (model->GetMaterials()[0].hasTexture) {
		textureSRV = model->GetTexture();
		context->PSSetShaderResources(0, 1, &textureSRV);
	}

	/* Set Camera buffer - Vertex shader */
	cameraCb.cameraPos = camera->GetPosition();

	context->UpdateSubresource(cameraBuffer, 0, nullptr, &cameraCb, 0, 0);
	context->VSSetConstantBuffers(1, 1, &cameraBuffer);
	
	/* Set Default light buffer	- Pixel shader */
	/*lightCb.ambientLight = light->GetAmbientColor();
	lightCb.diffuseLight = light->GetDiffuseColor();
	lightCb.specularLight = light->GetSpecularColor();
	lightCb.lightPosition = light->GetLightPosition();
	lightCb.lightRange = light->GetLightRange();
	lightCb.attenuation = light->GetAttenuation();

	context->UpdateSubresource(lightBuffer, 0, nullptr, &lightCb, 0, 0);
	context->PSSetConstantBuffers(0, 1, &lightBuffer);*/

	/* Material buffer - Pixel shader */
	materialCb.ambientMtl = model->GetMaterials()[0].ambientMtl;
	materialCb.diffuseMtl = model->GetMaterials()[0].diffuseMtl;
	materialCb.specularMtl = model->GetMaterials()[0].specularMtl;
	materialCb.reflectionMtl = model->GetMaterials()[0].reflectionMtl;
	materialCb.shine = model->GetMaterials()[0].shine;
	materialCb.hasTexture = model->GetMaterials()[0].hasTexture;
	materialCb.hasReflection = model->GetMaterials()[0].hasReflection;

	context->UpdateSubresource(materialBuffer, 0, nullptr, &materialCb, 0, 0);
	context->PSSetConstantBuffers(1, 1, &materialBuffer);

	/* Deferred light buffer - Compute shader */
	deferredLightCb.ambientLight = light->GetAmbientColor();
	deferredLightCb.diffuseLight = light->GetDiffuseColor();
	deferredLightCb.specularLight = light->GetSpecularColor();
	deferredLightCb.lightPosition = light->GetLightPosition();
	deferredLightCb.lightRange = light->GetLightRange();
	deferredLightCb.attenuation = light->GetAttenuation();
	deferredLightCb.cameraPosition = camera->GetPosition();

	context->UpdateSubresource(deferredBuffer, 0, NULL, &deferredLightCb, 0, 0);
	context->CSSetConstantBuffers(0, 1, &deferredBuffer);

	return true;
}

bool Shader::SetConstantParticleCBuffers(ID3D11DeviceContext* context, ParticleSystem* particleSystem, int maxParticles, XMMATRIX projection, float quadLength)
{
	particleBuffer = particleSystem->GetParticleBuffer();

	// COMPUTE SHADER BUFFER
	constantParticleCb.maxParticles = maxParticles;

	context->UpdateSubresource(particleBuffer, 0, NULL, &constantParticleCb, 0, 0);
	context->CSSetConstantBuffers(2, 1, &constantVariablesBuffer);

	// GEOMETRY SHADER BUFFER
	constantResourceCb.quadLength = quadLength;
	constantResourceCb.projection = projection;

	context->UpdateSubresource(particleBuffer, 0, NULL, &constantResourceCb, 0, 0);
	context->GSSetConstantBuffers(1, 1, &constantResourceBuffer);

	return true;
}

void Shader::UpdateParticleCBuffers(ID3D11DeviceContext* context, ParticleSystem* particleSystem, XMMATRIX view, XMFLOAT3 cameraPosition, float deltaTime)
{
	context->CSSetUnorderedAccessViews(0, 1, &nullUav, NULL);

	// COMPUTE SHADER BUFFER
	dynamicParticleCb.deltaTime = deltaTime;

	D3D11_MAPPED_SUBRESOURCE mapResource;
	ZeroMemory(&mapResource, sizeof(mapResource));

	context->Map(dynamicVariablesBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource);

	CBDynamicParticles* ptr = static_cast<CBDynamicParticles*>(mapResource.pData);

	memcpy(ptr, &dynamicParticleCb, sizeof(CBDynamicParticles));

	context->Unmap(dynamicVariablesBuffer, 0);

	// GEOMETRY SHADER BUFFER
	viewMatrixCb.view = view;
	viewMatrixCb.cameraPosition = cameraPosition;
	viewMatrixCb.time = deltaTime;

	ZeroMemory(&mapResource, sizeof(mapResource));

	context->Map(viewBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource);

	CBView* ptrVMB = static_cast<CBView*>(mapResource.pData);
	memcpy(ptrVMB, &viewMatrixCb, sizeof(CBView));

	context->Unmap(viewBuffer, 0);
}

void Shader::UpdateParticleCompute(ID3D11DeviceContext* context, ParticleSystem* particleSystem)
{
	particleSrv = particleSystem->GetSrv();
	particleUav = particleSystem->GetUav();
	particleBuffer = particleSystem->GetParticleBuffer();

	context->CSSetShader(particleCS, NULL, 0);
	context->CSSetConstantBuffers(2, 1, &constantVariablesBuffer);
	context->CSSetConstantBuffers(3, 1, &dynamicVariablesBuffer);
	context->CSSetUnorderedAccessViews(1, 1, &particleUav, NULL);
	context->Dispatch(32, 1, 1);

	context->CSSetUnorderedAccessViews(0, 1, &nullUav, NULL);
	context->CSSetUnorderedAccessViews(1, 1, &nullUav, NULL);

	context->VSSetShaderResources(0, 1, &nullSingleSrv);
	context->VSSetShaderResources(0, 1, &particleSrv);
}

void Shader::RenderShader(ID3D11DeviceContext* context, int indexCount, ID3D11SamplerState* sampler)
{
	// Set the vertex input layout.
	context->IASetInputLayout(defaultLayout);

	// Set the vertex and pixel shaders that will be used to render all objects and the light.
	/*context->VSSetShader(vertexShader, NULL, 0);
	context->PSSetShader(pixelShader, NULL, 0);*/

	// Set deferred shaders
	context->VSSetShader(deferredVS, NULL, 0);
	context->PSSetShader(deferredPS, NULL, 0);
	// Compute shader is set in separate function
	context->GSSetShader(NULL, NULL, 0); // Set Geometry shader as NULL for later stage (e.g particle system)
	context->DSSetShader(NULL, NULL, 0);
	context->HSSetShader(NULL, NULL, 0);

	// Set the sampler state in the pixel shader.
	context->PSSetSamplers(0, 1, &sampler);

	// Render the objects.
	context->DrawIndexed(indexCount, 0, 0);

	// Set everything to null after draw call teehee
	context->VSSetShader(nullVS, NULL, 0);
	context->PSSetShader(nullPS, NULL, 0);

	context->VSSetConstantBuffers(0, 1, &nullBuffer);
	context->PSSetConstantBuffers(0, 1, &nullBuffer);
	context->PSSetShaderResources(0, BUFFER_COUNT, nullSrvArray);
}
