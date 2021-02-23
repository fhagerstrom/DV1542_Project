#include "LightShaderClass.h"

LightShader::LightShader()
{
	vertexShader = nullptr;
	pixelShader = nullptr;
	layout = nullptr;
	sampleStateWrap = nullptr;
	matrixBuffer = nullptr;
	lightBuffer = nullptr;
}

LightShader::LightShader(const LightShader&) 
{

}

LightShader::~LightShader()
{

}

bool LightShader::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;


	// Initialize the vertex and pixel shaders.
	result = InitializeShader(device, hwnd, L"Debug/LightVertexShader.cso", L"LightPixelShader.cso");
	if (!result)
	{
		return false;
	}

	return true;
}

bool LightShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, dx::XMMATRIX worldMatrix, dx::XMMATRIX viewMatrix,
	dx::XMMATRIX projectionMatrix, ID3D11ShaderResourceView* colorTexture, ID3D11ShaderResourceView* normalTexture,
	dx::XMFLOAT3 lightDirection)
{
	bool result;


	// Set the shader parameters that it will use for rendering
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, colorTexture, normalTexture, lightDirection);
	if (!result)
	{
		return false;
	}

	// Render the prepared buffers with the shader
	RenderShader(deviceContext, indexCount);

	return true;
}

bool LightShader::InitializeShader(ID3D11Device* device, HWND hwnd, const WCHAR* vsFile, const WCHAR* psFile)
{
	unsigned int layoutElements;
	HRESULT hr;
	ID3DBlob* errorMessage;
	ID3DBlob* vertexShaderBuffer;
	ID3DBlob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC inputLayout[2];

	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;

	// Initialize the pointers this function will use to null.
	errorMessage = nullptr;
	vertexShaderBuffer = nullptr;
	pixelShaderBuffer = nullptr;
	
	hr = D3DCompileFromFile(vsFile, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "LightVertexShader", "vs_5_0", 0, 0,
		&vertexShaderBuffer, &errorMessage);

	if (FAILED(hr))
	{
		// If the shader failed to compile
		if (errorMessage)
		{
			LogLightShaderErrors(errorMessage, hwnd, vsFile);
		}

		// If the errorMessage doesn't contain anything, it couldn't find the file
		else
		{
			MessageBox(hwnd, vsFile, L"Vertex shader file not found!", MB_OK);
		}

		return false;
	}

	hr = D3DCompileFromFile(psFile, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "LightPixelShader", "ps_5_0", 0, 0,
		&pixelShaderBuffer, &errorMessage);

	if (FAILED(hr))
	{
		// If the shader failed to compile
		if (errorMessage)
		{
			LogLightShaderErrors(errorMessage, hwnd, psFile);
		}

		// If the errorMessage doesn't contain anything, it couldn't find the file
		else
		{
			MessageBox(hwnd, psFile, L"Pixel shader file not found!", MB_OK);
		}

		return false;
	}

	// Create the vertex shader from the buffer.
	hr = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &vertexShader);
	if (FAILED(hr))
	{
		return false;
	}

	// Create the pixel shader from the buffer.
	hr = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &pixelShader);
	if (FAILED(hr))
	{
		return false;
	}

	// Create input layout. Only needs position and texture coordinates -> 2D processing
	// Vertex
	inputLayout[0].SemanticName = "POSITION";
	inputLayout[0].SemanticIndex = 0;
	inputLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputLayout[0].InputSlot = 0;
	inputLayout[0].AlignedByteOffset = 0;
	inputLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputLayout[0].InstanceDataStepRate = 0;

	inputLayout[0].SemanticName = "TEXCOORD";
	inputLayout[0].SemanticIndex = 0;
	inputLayout[0].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputLayout[0].InputSlot = 0;
	inputLayout[0].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	inputLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputLayout[0].InstanceDataStepRate = 0;

	// Count amount of elements in the layout
	layoutElements = sizeof(inputLayout) / sizeof(inputLayout[0]);

	// Create vertex input layout
	hr = device->CreateInputLayout(inputLayout, layoutElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(),
		&layout);

	if (FAILED(hr))
	{
		return false;
	}

	// Release vertex shader and pixel shader buffers since they're no longer needed anymore
	vertexShaderBuffer->Release();
	pixelShaderBuffer->Release();

	// Create a sampler state
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = device->CreateSamplerState(&samplerDesc, &sampleStateWrap);
	if (FAILED(hr))
	{
		return false;
	}

	// Setup the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer for access to the vertex shader constant buffer from within this class.
	hr = device->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);
	if (FAILED(hr))
	{
		return false;
	}

	// Setup the dynamic light constant buffer that is in the pixel shader.
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	hr = device->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

void LightShader::ReleaseShader()
{
	// Release the light constant buffer.
	if (lightBuffer)
	{
		lightBuffer->Release();
	}

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

void LightShader::LogLightShaderErrors(ID3DBlob* errorMessage, HWND hwnd, const WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize;
	std::ofstream fout;

	compileErrors = (char*)(errorMessage->GetBufferPointer());

	bufferSize = errorMessage->GetBufferSize();

	fout.open("LightShader_errorlog.txt");

	for (unsigned int i = 0; i < bufferSize; ++i)
	{
		fout << compileErrors[i];
	}

	fout.close();

	errorMessage->Release();

	MessageBox(hwnd, L"Error compiling light shader. Check shader error log text file for more information.", shaderFilename, MB_OK);

	return;
}

bool LightShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, dx::XMMATRIX worldMatrix, dx::XMMATRIX viewMatrix, dx::XMMATRIX projectionMatrix, 
	ID3D11ShaderResourceView* colorTexture, ID3D11ShaderResourceView* normalTexture, dx::XMFLOAT3 lightDirection)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNr;
	MatrixBufferType* matrixData;
	LightBufferType* lightData;

	// ---------------------------------------------------------------------------------------------
	// MATRIX
	// ---------------------------------------------------------------------------------------------

	// Transpose matrices for shader
	dx::XMMatrixTranspose(worldMatrix);
	dx::XMMatrixTranspose(viewMatrix);
	dx::XMMatrixTranspose(projectionMatrix);

	// Lock constant buffer for writing
	hr = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	if (FAILED(hr))
	{
		return false;
	}

	// Pointer to data in matrix constant buffer
	matrixData = (MatrixBufferType*)mappedResource.pData;

	// Copy matrices to constant buffer
	matrixData->world = worldMatrix;
	matrixData->view = viewMatrix;
	matrixData->projection = projectionMatrix;

	// Unlock contant buffer
	deviceContext->Unmap(matrixBuffer, 0);

	// Set light constant buffer position
	bufferNr = 0;

	// Set light constant buffer to vertex shader
	deviceContext->VSSetConstantBuffers(bufferNr, 1, &lightBuffer);

	// ---------------------------------------------------------------------------------------------
	// LIGHT
	// ---------------------------------------------------------------------------------------------

	// Set shader texture resources in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &colorTexture);
	deviceContext->PSSetShaderResources(1, 1, &normalTexture);

	// Lock the light constant buffer so it can be written to.
	hr = deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	lightData = (LightBufferType*)mappedResource.pData;

	// Copy the lighting variables into the constant buffer.
	lightData->lightDirection = lightDirection;
	lightData->padding = 0;

	// Unlock the constant buffer.
	deviceContext->Unmap(lightBuffer, 0);

	// Set the position of the light constant buffer in the pixel shader.
	bufferNr = 0;

	// Finally set the light constant buffer in the pixel shader with the updated values.
	deviceContext->PSSetConstantBuffers(bufferNr, 1, &lightBuffer);

	// ---------------------------------------------------------------------------------------------

    return true;
}

void LightShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set vertex input layout
	deviceContext->IASetInputLayout(layout);
	
	// Set vertex and pixel shader
	deviceContext->VSSetShader(vertexShader, NULL, 0);
	deviceContext->PSSetShader(pixelShader, NULL, 0);

	// Set sampler state
	deviceContext->PSSetSamplers(0, 1, &sampleStateWrap);

	// Render geometry
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}
