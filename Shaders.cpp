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

bool Shader::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	// Initialize the vertex and pixel shaders.
	result = InitializeShader(device, hwnd, L"DefaultVS.hlsl", L"DefaultPS.hlsl");
	if(!result)
	{
		return false;
	}

	return true;
}


void Shader::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects.
	ShutdownShader();

	return;
}

bool Shader::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, 
					ID3D11ShaderResourceView* texture, XMFLOAT3 lightDirection, XMFLOAT4 ambient, XMFLOAT4 diffuse, 
					XMFLOAT3 cameraPos, XMFLOAT4 specular, float specularPower)
{
	bool result;

	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, lightDirection, ambient, diffuse, cameraPos, specular, specularPower);
	if(!result)
	{
		return false;
	}

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, indexCount);

	return true;
}

bool Shader::NewRender(ID3D11DeviceContext* context, Model* model, XMMATRIX view, XMMATRIX projection, Camera* camera, Light* light, ID3D11SamplerState* sampler)
{
	bool result;
	result = SetCBuffers(context, model, view, projection, camera, light);
	if (!result) {
		return false;
	}

	NewRenderShader(context, model->GetIndexCount(), sampler);

	return true;
}


bool Shader::InitializeShader(ID3D11Device* device, HWND hwnd, LPCWSTR vsFilename, LPCWSTR psFilename)
{
	HRESULT hr;
	ID3DBlob* vertexShaderBuffer;
	ID3DBlob* pixelShaderBuffer;
	D3D11_BUFFER_DESC perObjectBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC cameraBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;

	// Initialize the pointers this function will use to null.
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

    // Compile the vertex shader code.
	hr = D3DCompileFromFile(vsFilename, NULL, NULL, "main", "vs_5_0", NULL, 0, &vertexShaderBuffer, NULL);
	if(FAILED(hr))
	{
		MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		return false;
	}

    // Compile the pixel shader code.
	hr = D3DCompileFromFile(psFilename, NULL, NULL, "main", "ps_5_0", NULL, 0, &pixelShaderBuffer, NULL);
	if(FAILED(hr))
	{
		MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		return false;
	}

    // Create the vertex shader from the buffer.
    hr = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &vertexShader);
	if(FAILED(hr))
	{
		return false;
	}

    // Create the pixel shader from the buffer.
    hr = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &pixelShader);
	if(FAILED(hr))
	{
		return false;
	}

	D3D11_INPUT_ELEMENT_DESC inputLayoutPosTexNormal[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,	0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	hr = device->CreateInputLayout(inputLayoutPosTexNormal, ARRAYSIZE(inputLayoutPosTexNormal), vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &defaultLayout);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Couldn't create input layout!", L"SHADER ERROR", MB_OK);
		return false;
	}

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

    // Setup the description of the matrix constant buffer that is in the vertex shader.
    perObjectBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	perObjectBufferDesc.ByteWidth = sizeof(CBPerObject);
    perObjectBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    perObjectBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    perObjectBufferDesc.MiscFlags = 0;
	perObjectBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	hr = device->CreateBuffer(&perObjectBufferDesc, NULL, &perObjectBuffer);
	if(FAILED(hr))
	{
		return false;
	}

	// Setup the description of the camera dynamic constant buffer that is in the vertex shader.
	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CBCamera);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;

	// Create the camera constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	hr = device->CreateBuffer(&cameraBufferDesc, NULL, &cameraBuffer);
	if (FAILED(hr))
	{
		return false;
	}

	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(CBLight);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	hr = device->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);
	if (FAILED(hr))
	{
		return false;
	}

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	hr = device->CreateSamplerState(&samplerDesc, &samplerState);
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

bool Shader::InitializeShaders(ID3D11Device* device, HWND hwnd, LPCWSTR vsFileName, LPCWSTR psFileName, LPCSTR VSEntry, LPCSTR PSEntry)
{
	/* COMPILE AND CERATE VERTEX AND PIXEL SHADERS */

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
	pixelShaderBlob->Release();

	return true;
}

bool Shader::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
								XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT3 lightDirection, XMFLOAT4 ambient, XMFLOAT4 diffuse, 
								XMFLOAT3 cameraPos, XMFLOAT4 specular, float specularPower)
{
	HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
	CBPerObject* dataPtr;
	CBLight* dataPtr2;
	CBCamera* dataPtr3;

	// Transpose the matrices to prepare them for the shader.
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(perObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr = (CBPerObject*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world = worldMatrix;
	/*dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;*/


	// Unlock the constant buffer.
    deviceContext->Unmap(perObjectBuffer, 0);

	// Finanly set the constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(0, 1, &perObjectBuffer);

	// Lock the camera constant buffer so it can be written to.
	result = deviceContext->Map(cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr3 = (CBCamera*)mappedResource.pData;

	// Copy the camera position into the constant buffer.
	dataPtr3->cameraPos = cameraPos;
	dataPtr3->padding = 0.0f;

	// Unlock the camera constant buffer.
	deviceContext->Unmap(cameraBuffer, 0);

	// Now set the camera constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(1, 1, &cameraBuffer);

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);

	// Lock the light constant buffer so it can be written to.
	result = deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr2 = (CBLight*)mappedResource.pData;

	// Copy the lighting variables into the constant buffer.
	dataPtr2->ambientLight = ambient;
	dataPtr2->diffuseLight = diffuse;
	dataPtr2->specularLight = specular;
	dataPtr2->lightDirection = lightDirection;
	dataPtr2->specularPower = specularPower;

	// Unlock the constant buffer.
	deviceContext->Unmap(lightBuffer, 0);

	// Finally set the light constant buffer in the pixel shader with the updated values.
	deviceContext->PSSetConstantBuffers(0, 1, &lightBuffer);

	return true;
}


bool Shader::SetCBuffers(ID3D11DeviceContext* context, Model* model, DirectX::XMMATRIX view, DirectX::XMMATRIX projection, Camera* camera, Light* light)
{
	// OBJECT / MATRIX BUFFERS
	XMMATRIX worldViewProjection;
	worldViewProjection = model->GetWorldMatrix() * view * projection;

	objectCb.wvp = XMMatrixTranspose(worldViewProjection);
	objectCb.world = DirectX::XMMatrixTranspose(model->GetWorldMatrix());

	context->UpdateSubresource(perObjectBuffer, 0, nullptr, &perObjectBuffer, 0, 0);
	context->VSSetConstantBuffers(0, 1, &perObjectBuffer);

	// Set shader texture resource in the pixel shader	
	/*if (model->GetMaterials()[0].hasTexture) {
		textureSRV = model->GetTexture();
		context->PSSetShaderResources(0, 1, &textureSRV);
	}*/

	/* Set Camera buffer - Vertex shader */
	cameraCb.cameraPos = camera->GetPosition();

	context->UpdateSubresource(cameraBuffer, 0, nullptr, &cameraCb, 0, 0);
	context->VSSetConstantBuffers(1, 1, &cameraBuffer);
	
	/* Set Light buffer	- Pixel shader */
	lightCb.ambientLight = light->GetAmbientColor();
	lightCb.diffuseLight = light->GetDiffuseColor();
	lightCb.specularLight = light->GetSpecularColor();
	lightCb.lightDirection = light->GetDirection();

	context->UpdateSubresource(lightBuffer, 0, nullptr, &lightCb, 0, 0);
	context->PSSetConstantBuffers(0, 1, &lightBuffer);

	/* Material buffer - Pixel shader */
	/*materialCb.ambientMtl = model->GetMaterials()[0].ambientMtl;
	materialCb.diffuseMtl = model->GetMaterials()[0].diffuseMtl;
	materialCb.specularMtl = model->GetMaterials()[0].specularMtl;
	materialCb.reflectionColor = model->GetMaterials()[0].reflectionMtl;
	materialCb.shine = model->GetMaterials()[0].shine;
	materialCb.hasTexture = model->GetMaterials()[0].hasTexture;
	materialCb.hasReflection = model->GetMaterials()[0].hasReflection;
	materialCb.translation = model->GetMaterials()[0].translation;*/

	context->UpdateSubresource(materialBuffer, 0, nullptr, &materialCb, 0, 0);
	context->PSSetConstantBuffers(1, 1, &materialBuffer);

	return true;
}

void Shader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(defaultLayout);

    // Set the vertex and pixel shaders that will be used to render this triangle.
    deviceContext->VSSetShader(vertexShader, NULL, 0);
    deviceContext->PSSetShader(pixelShader, NULL, 0);

	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &samplerState);

	// Render the object.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}

void Shader::NewRenderShader(ID3D11DeviceContext* deviceContext, int indexCount, ID3D11SamplerState* sampler)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(defaultLayout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(vertexShader, NULL, 0);
	deviceContext->PSSetShader(pixelShader, NULL, 0);

	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &sampler);

	// Render the object.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}