#include "DeferredShader.h"

DeferredShader::DeferredShader()
{
	vertexShader = nullptr;
	pixelShader = nullptr;
	layout = nullptr;
	sampleStateWrap = nullptr;
	matrixBuffer = nullptr;
}

DeferredShader::DeferredShader(const DeferredShader&)
{
	
}

DeferredShader::~DeferredShader()
{

}

bool DeferredShader::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;


	// Initialize the vertex and pixel shaders.
	result = InitializeShader(device, hwnd, L"Debug/DeferredVertexShader.cso", L"Debug/DeferredPixelShader.cso");
	if (!result)
	{
		return false;
	}

	return true;
}

bool DeferredShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, dx::XMMATRIX worldMatrix, dx::XMMATRIX viewMatrix,
	dx::XMMATRIX projectionMatrix, ID3D11ShaderResourceView* textureSRV)
{
	bool result;


	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, textureSRV);
	if (!result)
	{
		return false;
	}

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, indexCount);

	return true;
}

bool DeferredShader::InitializeShader(ID3D11Device* device, HWND hwnd, const WCHAR* vsFile, const WCHAR* psFile)
{
	HRESULT hr;
	ID3DBlob* errorMessage = nullptr;
	ID3DBlob* vertexShaderBuffer = nullptr;
	ID3DBlob* pixelShaderBuffer = nullptr;
	D3D11_INPUT_ELEMENT_DESC inputLayout[3];
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc;
	unsigned int layoutElements;

	// Compile vertex shader
	hr = D3DCompileFromFile(vsFile, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "DefferedVertexShader", "vs_5_0", 0, 0,
		&vertexShaderBuffer, &errorMessage);

	if (FAILED(hr))
	{
		// If it failed to compile
		if (errorMessage)
		{
			LogDeferredShaderErrors(errorMessage, hwnd, vsFile);
		}

		// Otherwise it couldn't be found at all
		else
		{
			MessageBox(hwnd, vsFile, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// Compile pixel shader
	hr = D3DCompileFromFile(vsFile, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "DefferedPixelShader", "ps_5_0", 0, 0,
		&vertexShaderBuffer, &errorMessage);

	if (FAILED(hr))
	{
		// If it failed to compile
		if (errorMessage)
		{
			LogDeferredShaderErrors(errorMessage, hwnd, psFile);
		}

		// Otherwise it couldn't be found at all
		else
		{
			MessageBox(hwnd, psFile, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	hr = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &vertexShader);
	if (FAILED(hr))
	{
		return false;
	}

	hr = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &pixelShader);
	if (FAILED(hr))
	{
		return false;
	}

	// Create the vertex input layout description.
	inputLayout[0].SemanticName = "POSITION";
	inputLayout[0].SemanticIndex = 0;
	inputLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputLayout[0].InputSlot = 0;
	inputLayout[0].AlignedByteOffset = 0;
	inputLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputLayout[0].InstanceDataStepRate = 0;

	inputLayout[1].SemanticName = "TEXCOORD";
	inputLayout[1].SemanticIndex = 0;
	inputLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputLayout[1].InputSlot = 0;
	inputLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	inputLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputLayout[1].InstanceDataStepRate = 0;

	inputLayout[2].SemanticName = "NORMAL";
	inputLayout[2].SemanticIndex = 0;
	inputLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputLayout[2].InputSlot = 0;
	inputLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	inputLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputLayout[2].InstanceDataStepRate = 0;

	// Check amount of elements in layout
	layoutElements = sizeof(inputLayout) / sizeof(inputLayout[0]);

	// Create vertex input layout
	hr = device->CreateInputLayout(inputLayout, layoutElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &layout);

	if (FAILED(hr))
	{
		return false;
	}

	// Release buffers
	vertexShaderBuffer->Release();
	pixelShaderBuffer->Release();

	// Create a wrap texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create sampler state
	hr = device->CreateSamplerState(&samplerDesc, &sampleStateWrap);
	if (FAILED(hr))
	{
		return false;
	}

	// Setup matrix constant buffer in vertex shader
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create constant buffer pointer for vertex shader constant buffer
	hr = device->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

void DeferredShader::ReleaseShader()
{
	// Release the matrix constant buffer.
	if (matrixBuffer)
	{
		matrixBuffer->Release();
	}

	// Release the sampler state.
	if (sampleStateWrap)
	{
		sampleStateWrap->Release();
	}

	// Release the layout.
	if (layout)
	{
		layout->Release();
	}

	// Release the pixel shader.
	if (pixelShader)
	{
		pixelShader->Release();
	}

	// Release the vertex shader.
	if (vertexShader)
	{
		vertexShader->Release();
	}

	return;
}

void DeferredShader::LogDeferredShaderErrors(ID3DBlob* errorMessage, HWND hwnd, const WCHAR* shaderFile)
{
	char* compileErrors;
	unsigned long bufferSize;
	std::ofstream fout;

	// Get a pointer to the error message text buffer
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get message length
	bufferSize = errorMessage->GetBufferSize();

	// Open a file for message
	fout.open("DeferredShader_errorlog.txt");

	// Write out the error message
	for (unsigned int i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// Close file and release error message
	fout.close();
	errorMessage->Release();

	// Message box to notify to check the text file for compile errors
	MessageBox(hwnd, L"Error compiling deferred shader. Check Deferred Shader log for message.", shaderFile, MB_OK);

	return;

}


bool DeferredShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, dx::XMMATRIX worldMatrix, dx::XMMATRIX viewMatrix, dx::XMMATRIX projectionMatrix, ID3D11ShaderResourceView* textureSRV)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNr;
	MatrixBuffer* matrixData;

	// Transpose matrices to prepare for shader
	dx::XMMatrixTranspose(worldMatrix);
	dx::XMMatrixTranspose(viewMatrix);
	dx::XMMatrixTranspose(projectionMatrix);

	// Lock constant buffer for writing
	hr = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr))
	{
		return false;
	}

	// Pointer to constant buffer data
	matrixData = (MatrixBuffer*)mappedResource.pData;

	// Copy matrices into constant buffer
	matrixData->world = worldMatrix;
	matrixData->view = viewMatrix;
	matrixData->projection = projectionMatrix;

	// Unlock constant buffer
	deviceContext->Unmap(matrixBuffer, 0);

	// Set position of constant buffer in vertex shader
	bufferNr = 0;

	// Set the constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNr, 1, &matrixBuffer);

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &textureSRV);

	return true;
}

void DeferredShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(layout);

	// Set the vertex and pixel shaders that will be used to render.
	deviceContext->VSSetShader(vertexShader, NULL, 0);
	deviceContext->PSSetShader(pixelShader, NULL, 0);

	// Set the sampler states in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &sampleStateWrap);

	// Render the geometry.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}




