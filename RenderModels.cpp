#include "Defines.h"
#include "RenderModels.h"



RenderModels::RenderModels(ID3D11Device* device)
	:matrixData(), pixelShader(nullptr), vertexShader(nullptr)
{
	CreateConstantBuffer(device, matrixBuffer, sizeof(matrixData));
	CreateConstantBuffer(device, lightingBuffer, sizeof(XMMATRIX));
	CreateConstantBuffer(device, tessellBuffer, sizeof(TessellationBuffer));

	std::string shaderData;
	std::ifstream reader;
	std::string shaderByteCode;
	
	modelsRendered = 0;

	// PIXEL SHADER
	reader.open(ps_path, std::ios::binary | std::ios::beg);

	if (!reader.is_open())
	{
		std::cout << "ERROR! COULD NOT OPEN FILE!: " + ps_path << std::endl;
		return;
	}

	reader.seekg(0, std::ios::end);
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
	reader.seekg(0, std::ios::beg);
	shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());

	HRESULT hr = device->CreatePixelShader(shaderData.c_str(), shaderData.length(), nullptr, &pixelShader);
	if FAILED(hr)
	{
		MessageBox(NULL, L"Couldn't create pixel shader!", L"SHADER ERROR", MB_OK);
		return;
	}

	shaderData.clear();
	reader.close();

	// VERTEX SHADER
	reader.open(vs_path, std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		MessageBox(NULL, L"Couldn't read the path to the vertex shader!", L"READ ERROR", MB_OK);
		return;
	}

	reader.seekg(0, std::ios::end);
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
	reader.seekg(0, std::ios::beg);

	shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());

	if (FAILED(device->CreateVertexShader(shaderData.c_str(), shaderData.length(), nullptr, &vertexShader)))
	{
		MessageBox(NULL, L"Couldn't create the vertex shader!", L"SHADER ERROR", MB_OK);
		return;
	}

	byteCode = shaderData;

	shaderData.clear();
	reader.close();

	// DOMAIN SHADER
	reader.open(dsPath, std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		MessageBox(NULL, L"Couldn't read path to the domain shader!", L"SHADER ERROR", MB_OK);
		return;
	}
	reader.seekg(0, std::ios::end);
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
	reader.seekg(0, std::ios::beg);
	shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());
	hr = device->CreateDomainShader(shaderData.c_str(), shaderData.length(), nullptr, &domainShader);

	if (FAILED(hr))
	{
		MessageBox(NULL, L"Couldn't create the domain shader!", L"SHADER ERROR", MB_OK);
		return;
	}

	shaderData.clear();
	reader.close();

	// HULL SHADER
	reader.open(hsPath, std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		MessageBox(NULL, L"Couldn't read path to the hull shader!", L"SHADER ERROR", MB_OK);
		return;
	}

	reader.seekg(0, std::ios::end);
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
	reader.seekg(0, std::ios::beg);
	shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());
	hr = device->CreateHullShader(shaderData.c_str(), shaderData.length(), nullptr, &hullShader);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Couldn't create the hull shader!", L"SHADER ERROR", MB_OK);
		return;
	}

	shaderData.clear();
	reader.close();
}


void RenderModels::Render(ID3D11Device* device, ID3D11DeviceContext* context, Model* model, bool rotation, float tessellAmount)
{
	// Get WorldMatrix, store and update buffer for models
	XMStoreFloat4x4(&matrixData.worldSpace, XMMatrixTranspose(model->GetWorldMatrix()));
	XMMATRIX wvp = XMMatrixTranspose(model->GetWorldMatrix() * Shaders::viewMatrix * Shaders::projectionMatrix); // Local to clipspace
	XMStoreFloat4x4(&matrixData.wvp, wvp);
	UpdateBuffer(context, matrixBuffer, matrixData);

	context->IASetInputLayout(Shaders::modelLayout);
	context->VSSetShader(vertexShader, NULL, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);

	// Pixel shader for every model
	context->PSSetShader(pixelShader, NULL, 0);

	// Update the light buffer for every model. This is also used for the shadows 
	UpdateBuffer(context, lightingBuffer, Shaders::lightingMatrix);
	context->PSSetConstantBuffers(1, 1, &lightingBuffer); // Set constant buffer containing light info
	model->BindTextures(context); // Set shader resource
	context->VSSetConstantBuffers(1, 1, &matrixBuffer); // Set constant buffer for the vertex shader with the WVP and worldspace

	// Set matrix Cbuffer in the domain shader and update tesellation buffer
	context->DSSetConstantBuffers(2, 1, &matrixBuffer);

	// Copy tessellation data into constant buffer
	tessellData.tessellAmount = tessellAmount;
	tessellData.padding = XMFLOAT3(0.0f, 0.0f, 0.0f);

	UpdateBuffer(context, tessellBuffer, tessellData);
	context->HSSetConstantBuffers(2, 1, &tessellBuffer);

	// DS and HS are used for tessellation
	context->HSSetShader(hullShader, NULL, 0);
	context->DSSetShader(domainShader, NULL, 0);

	// GS is already set outside the renderer

	// Get, update and set the MTL data for every model
	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
	HRESULT hr = context->Map(*model->GetMTLBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
	if FAILED(hr)
	{
		MessageBox(NULL, L"Couldn't get the material buffer for the model!", L"BUFFER ERROR", MB_OK);
		return;
	}

	memcpy(mappedBuffer.pData, &model->GetMaterial(), sizeof(model->GetMaterial()));
	context->Unmap(*model->GetMTLBuffer(), 0);

	modelsRendered++; // For printouts to control frustum culling

	context->PSSetConstantBuffers(0, 1, model->GetMTLBuffer());
	context->IASetVertexBuffers(0, 1, model->GetBuffer(), &stride, &offset);																		
	context->Draw(model->GetVertexCount(), 0);
}


std::string RenderModels::GetByteCode() const
{
	return this->byteCode; 
}

void RenderModels::ShutDown()
{
	pixelShader->Release();
	vertexShader->Release();
	matrixBuffer->Release();
	lightingBuffer->Release();
}