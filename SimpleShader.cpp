#include "SimpleShader.h"

SimpleShaderHandler::SimpleShaderHandler(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	this->device = device;
	this->deviceContext = deviceContext;

	constantBufferCount = 0;
	constantBuffers = 0;
	shaderBlob = 0;
}

SimpleShaderHandler::~SimpleShaderHandler()
{
	// Derived class destructors call this class' CleanUp() function
	if (shaderBlob)
		shaderBlob->Release();
}

// Cleans up variables and buffers. Other data will be handled by derived classes.
void SimpleShaderHandler::CleanUp()
{
	for (unsigned int i = 0; i < constantBufferCount; ++i)
	{
		constantBuffers[i].constantBuffer->Release();
		delete[] constantBuffers[i].LocalDataBuffer;
	}

	if (constantBuffers)
	{
		delete[] constantBuffers;
		constantBufferCount = 0;
	}

	for (unsigned int i = 0; i < shaderResourceViews.size(); ++i)
		delete[] shaderResourceViews[i];

	for (unsigned int i = 0; i < samplerStates.size(); ++i)
		delete[] samplerStates[i];

	varTable.clear();
	cbTable.clear();
	samplerTable.clear();
	textureTable.clear();
}

bool SimpleShaderHandler::LoadShaderFiles(LPCWSTR shaderFile)
{
	HRESULT hr = D3DReadFileToBlob(shaderFile, &shaderBlob);
	if (hr != S_OK)
	{
		return false;
	}

	isShaderValid = CreateShader(shaderBlob);
	if (!isShaderValid)
	{
		return false;
	}

	ID3D11ShaderReflection* reflection;
	D3DReflect(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&reflection);

	D3D11_SHADER_DESC shaderDesc;
	reflection->GetDesc(&shaderDesc);

	// Resource arrays
	constantBufferCount = shaderDesc.ConstantBuffers;
	constantBuffers = new SimpleConstantBuffer[constantBufferCount];

	// Handle resources
	unsigned int resourceCount = shaderDesc.BoundResources;
	for (unsigned int i = 0; i < resourceCount; ++i)
	{
		// Resource description
		D3D11_SHADER_INPUT_BIND_DESC resourceDesc;
		reflection->GetResourceBindingDesc(i, &resourceDesc);

		switch (resourceDesc.Type)
		{
			case D3D_SIT_TEXTURE: // Texture resource
			{
				SimpleSRV* srv = new SimpleSRV();
				srv->bindIndex = resourceDesc.BindPoint;
				srv->index = shaderResourceViews.size();

				textureTable.insert(std::pair<std::string, SimpleSRV*>(resourceDesc.Name, srv));
				shaderResourceViews.push_back(srv);
			}

				break;

			case D3D_SIT_SAMPLER: // Sampler resource
			{
				SimpleSampler* sampler = new SimpleSampler();
				sampler->bindIndex = resourceDesc.BindPoint;
				sampler->index = samplerStates.size();

				samplerTable.insert(std::pair<std::string, SimpleSampler*>(resourceDesc.Name, sampler));
				samplerStates.push_back(sampler);
			}
				break;
		}
	}

	for (unsigned int b = 0; b < constantBufferCount; ++b)
	{
		// Get buffer
		ID3D11ShaderReflectionConstantBuffer* cb = reflection->GetConstantBufferByIndex(b);

		// Get buffer description
		D3D11_SHADER_BUFFER_DESC bufferDesc;
		cb->GetDesc(&bufferDesc);

		// Get resource description, how is it binded to the shader?
		D3D11_SHADER_INPUT_BIND_DESC bindDesc;
		reflection->GetResourceBindingDescByName(bindDesc.Name, &bindDesc);

		// Set buffer and add pointer to table
		constantBuffers[b].BindIndex = bindDesc.BindPoint;
		constantBuffers[b].Name = bufferDesc.Name;

		cbTable.insert(std::pair<std::string, SimpleConstantBuffer*>(bufferDesc.Name, &constantBuffers[b]));

		// Create this constant buffer.
		D3D11_BUFFER_DESC newBuffDesc;
		newBuffDesc.Usage = D3D11_USAGE_DEFAULT;
		newBuffDesc.ByteWidth = bufferDesc.Size;
		newBuffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		newBuffDesc.CPUAccessFlags = 0;
		newBuffDesc.MiscFlags = 0;
		newBuffDesc.StructureByteStride = 0;
		device->CreateBuffer(&newBuffDesc, 0, &constantBuffers[b].constantBuffer);

		// Set up the data buffer for this constant buffer.
		constantBuffers[b].size = bufferDesc.Size;
		constantBuffers[b].LocalDataBuffer = new unsigned char[bufferDesc.Size];
		ZeroMemory(constantBuffers[b].LocalDataBuffer, bufferDesc.Size);

		// Loop through all variables in this buffer.
		for (unsigned int v = 0; v < bufferDesc.Variables; v++)
		{
			// Get this variable.
			ID3D11ShaderReflectionVariable* var =
				cb->GetVariableByIndex(v);

			// Get the description of the variable and its type.
			D3D11_SHADER_VARIABLE_DESC varDesc;
			var->GetDesc(&varDesc);

			// Create the variable struct.
			SimpleShaderVar varStruct;
			varStruct.constantBufferIndex = b;
			varStruct.byteOffset = varDesc.StartOffset;
			varStruct.size = varDesc.Size;

			// Get a string version.
			std::string varName(varDesc.Name);

			// Add this variable to the table and the constant buffer.
			varTable.insert(std::pair<std::string, SimpleShaderVar>(varName, varStruct));
			constantBuffers[b].variables.push_back(varStruct);
		}
	}

	// Done
	reflection->Release();
	return true;

}



