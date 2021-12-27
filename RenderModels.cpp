#include "Defines.h"
#include "RenderModels.h"



RenderModels::RenderModels(ID3D11Device* device)
	:matrix(), pixelShader(nullptr), vertexShader(nullptr)
{
	CreateBuffer(device, matrixBuffer, sizeof(matrix));
	CreateBuffer(device, lightingBuffer, sizeof(XMMATRIX));

	std::string shaderData;
	std::ifstream reader;
	std::string shaderByteCode;
	// PIXEL SHADER
	reader.open(ps_path, std::ios::binary | std::ios::beg);

	if (!reader.is_open())
	{
		std::cout << "COULD NOT OPEN FILE: " + ps_path << std::endl;
		return;
	}

	reader.seekg(0, std::ios::end);
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
	reader.seekg(0, std::ios::beg);
	shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());

	HRESULT hr = device->CreatePixelShader(shaderData.c_str(), shaderData.length(), nullptr, &pixelShader);
	if FAILED(hr)
	{
		std::cout << "FAILED TO CREATE PIXEL SHADER" << std::endl;
		return;
	}

	shaderData.clear();
	reader.close();

	// VERTEX SHADER
	reader.open(vs_path, std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		std::cerr << "Could not open VS file!" << std::endl;
		return;
	}

	reader.seekg(0, std::ios::end);
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
	reader.seekg(0, std::ios::beg);

	shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());

	if (FAILED(device->CreateVertexShader(shaderData.c_str(), shaderData.length(), nullptr, &vertexShader)))
	{
		std::cerr << "Failed to create pixel shader!" << std::endl;
		return;
	}

	byteCode = shaderData;

	shaderData.clear();
	reader.close();
}

void RenderModels::Render(ID3D11Device* device, ID3D11DeviceContext* context, Model* model, bool rotation)
{
	// Get the WorldMatrix, store and update the buffer for the model ( Transposed WorldMatrix HLSL is columnmayor )
	XMStoreFloat4x4(&matrix.worldSpace, XMMatrixTranspose(model->GetWorldMatrix()));
	XMMATRIX wvp = XMMatrixTranspose(model->GetWorldMatrix() * Shaders::viewMatrix * Shaders::perspectiveMatrix); // Local to clipspace (camera)
	XMStoreFloat4x4(&matrix.wvp, wvp);
	UpdateBuffer(context, matrixBuffer, matrix);

	// Set the inputlayout, VS and topology
	context->IASetInputLayout(Shaders::modelLayout);
	context->VSSetShader(vertexShader, NULL, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	
	// use pixelshader for each model
	context->PSSetShader(pixelShader, NULL, 0);
	

	// Update the lightbuffer for each model, used for the shadows (sent to the clipspace target in the PS)
	UpdateBuffer(context, lightingBuffer, Shaders::lightingMatrix);
	context->PSSetConstantBuffers(1, 1, &lightingBuffer); // set the CB containing light info
	model->BindTextures(context);	// Set the shader resource with the specific texture used for the modelm
	context->VSSetConstantBuffers(1, 1, &matrixBuffer); // set the CB for the vs with the WVP and worldspace matrices
	// DS and HS not used for regular models
	context->HSSetShader(NULL, NULL, 0);
	context->DSSetShader(NULL, NULL, 0);
	// GS already set outside the renderer

	// Get, update and set the MTL data for each model
	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
	HRESULT hr = context->Map(*model->GetMTLBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
	if FAILED(hr)
	{
		std::cout << "FAILED TO UPDATE MTL BUFFER" << std::endl;
		return;
	}
	memcpy(mappedBuffer.pData, &model->GetMaterial(), sizeof(model->GetMaterial()));
	context->Unmap(*model->GetMTLBuffer(), 0);

	context->PSSetConstantBuffers(0, 1, model->GetMTLBuffer());
	context->IASetVertexBuffers(0, 1, model->GetBuffer(), &stride, &offset);				// Sends vertex buffer, per model, to the input assembly 
																							// ( first stage of the graphics pipeline ) that tells the vertex 
																							// shader how to read the data
	context->Draw(model->GetVertexCount(), 0);
}

void RenderModels::ShutDown()
{
	pixelShader->Release();
	vertexShader->Release();
	matrixBuffer->Release();
	lightingBuffer->Release();
}