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
		delete[] constantBuffers[i].localDataBuffer;
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

SimpleShaderVar* SimpleShaderHandler::FindVariable(std::string name, int size)
{
	// Look for the key
	std::unordered_map<std::string, SimpleShaderVar>::iterator result =
		varTable.find(name);

	// If the key was found...
	if (result == varTable.end())
		return 0;

	// Grab the result from the iterator.
	SimpleShaderVar* var = &(result->second);

	// Check if data size is correct.
	if (size > 0 && var->size != size)
		return 0;

	// Success.
	return var;
}

SimpleConstantBuffer* SimpleShaderHandler::FindConstantBuffer(std::string name)
{
	// Look for the key
	std::unordered_map<std::string, SimpleConstantBuffer*>::iterator result = cbTable.find(name);

	// If the key was found...
	if (result == cbTable.end())
		return 0;

	// Success.
	return result->second;
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
		reflection->GetResourceBindingDescByName(bufferDesc.Name, &bindDesc);

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
		constantBuffers[b].localDataBuffer = new unsigned char[bufferDesc.Size];
		ZeroMemory(constantBuffers[b].localDataBuffer, bufferDesc.Size);

		// Loop through all variables in the buffer.
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

void SimpleShaderHandler::SetShader()
{
	// Ensure the shader is valid
	if (!isShaderValid) return;

	// Set the shader and any relevant constant buffers, which
	// is an overloaded method in a subclass
	SetShaderAndCBs();
}

void SimpleShaderHandler::CopyAllBufferData()
{
	// Ensure the shader is valid
	if (!isShaderValid) return;

	// Loop through the constant buffers and copy all data
	for (unsigned int i = 0; i < constantBufferCount; i++)
	{
		// Copy the entire local data buffer
		deviceContext->UpdateSubresource(
			constantBuffers[i].constantBuffer, 0, 0,
			constantBuffers[i].localDataBuffer, 0, 0);
	}
}

// Copies local shader data to corresponding constant buffer.
// Get the index of the buffer. 
// Doing this because we update variables that change often without re-copying the buffers. 
void SimpleShaderHandler::CopyBufferData(unsigned int index)
{
	// Ensure the shader is valid
	if (!isShaderValid) return;

	// Validate the index
	if (index >= this->constantBufferCount)
		return;

	// Check for the buffer
	SimpleConstantBuffer* cb = &this->constantBuffers[index];
	if (!cb) 
		return;

	// Copy the data and get out
	deviceContext->UpdateSubresource(cb->constantBuffer, 0, 0, cb->localDataBuffer, 0, 0);
}

void SimpleShaderHandler::CopyBufferData(std::string bufferName)
{
	// Ensure the shader is valid
	if (!isShaderValid) return;

	// Check for the buffer
	SimpleConstantBuffer* cb = this->FindConstantBuffer(bufferName);
	if (!cb) 
		return;

	// Copy the data and get out
	deviceContext->UpdateSubresource(cb->constantBuffer, 0, 0, cb->localDataBuffer, 0, 0);
}



// Set arbitrary shader data.
bool SimpleShaderHandler::SetData(std::string name, const void* data, unsigned int size)
{
	// Look for the variable and verify
	SimpleShaderVar* var = FindVariable(name, size);
	if (var == 0)
		return false;

	// Set the data in the local data buffer
	memcpy(constantBuffers[var->constantBufferIndex].localDataBuffer + var->byteOffset, data, size);

	// Success
	return true;
}


bool SimpleShaderHandler::SetInt(std::string name, int data)
{
	return this->SetData(name, (void*)(&data), sizeof(int));
}

bool SimpleShaderHandler::SetFloat(std::string name, float data)
{
	return this->SetData(name, (void*)(&data), sizeof(float));
}

bool SimpleShaderHandler::SetFloat2(std::string name, const float data[2])
{
	return this->SetData(name, (void*)data, sizeof(float) * 2);
}

bool SimpleShaderHandler::SetFloat2(std::string name, const DirectX::XMFLOAT2 data)
{
	return this->SetData(name, &data, sizeof(float) * 2);
}

bool SimpleShaderHandler::SetFloat3(std::string name, const float data[3])
{
	return this->SetData(name, (void*)data, sizeof(float) * 3);
}

bool SimpleShaderHandler::SetFloat3(std::string name, const DirectX::XMFLOAT3 data)
{
	return this->SetData(name, &data, sizeof(float) * 3);
}

bool SimpleShaderHandler::SetFloat4(std::string name, const float data[4])
{
	return this->SetData(name, (void*)data, sizeof(float) * 4);
}

bool SimpleShaderHandler::SetFloat4(std::string name, const DirectX::XMFLOAT4 data)
{
	return this->SetData(name, &data, sizeof(float) * 4);
}

bool SimpleShaderHandler::SetMatrix4x4(std::string name, const float data[16])
{
	return this->SetData(name, (void*)data, sizeof(float) * 16);
}

bool SimpleShaderHandler::SetMatrix4x4(std::string name, const DirectX::XMFLOAT4X4 data)
{
	return this->SetData(name, &data, sizeof(float) * 16);
}

const SimpleShaderVar* SimpleShaderHandler::GetVariableInfo(std::string name)
{
	return FindVariable(name, -1);
}

const SimpleSRV* SimpleShaderHandler::GetShaderResourceViewInfo(std::string name)
{
	// Look for the key.
	std::unordered_map<std::string, SimpleSRV*>::iterator result = textureTable.find(name);

	// If we found the key...
	if (result == textureTable.end())
		return 0;

	// Successful.
	return result->second;
}

const SimpleSRV* SimpleShaderHandler::GetShaderResourceViewInfo(unsigned int index)
{
	// Check if index is valid.
	if (index >= shaderResourceViews.size()) 
		return 0;

	// Grab the bind index.
	return shaderResourceViews[index];
}

const SimpleSampler* SimpleShaderHandler::GetSamplerInfo(std::string name)
{
	// Look for the key.
	std::unordered_map<std::string, SimpleSampler*>::iterator result = samplerTable.find(name);

	// If we found the key...
	if (result == samplerTable.end())
		return 0;

	// Successful.
	return result->second;
}

const SimpleSampler* SimpleShaderHandler::GetSamplerInfo(unsigned int index)
{
	// Check if index is valid.
	if (index >= samplerStates.size()) return 0;

	// Grab the bind index.
	return samplerStates[index];
}

unsigned int SimpleShaderHandler::GetBufferCount()
{
	return constantBufferCount;
}

unsigned int SimpleShaderHandler::GetBufferSize(unsigned int index)
{
	// Check if index is valid. If not, return -1
	if (index >= constantBufferCount)
		return -1;

	// Grab the size
	return constantBuffers[index].size;
}

const SimpleConstantBuffer* SimpleShaderHandler::GetBufferInfo(std::string name)
{
	return FindConstantBuffer(name);
}

const SimpleConstantBuffer* SimpleShaderHandler::GetBufferInfo(unsigned int index)
{
	// Check for valid index.
	if (index >= constantBufferCount) return 0;

	// Return the specific buffer.
	return &constantBuffers[index];
}

// *******************************************************
// SIMPLE VERTEX SHADER
// *******************************************************

SimpleVertexShader::SimpleVertexShader(ID3D11Device* device, ID3D11DeviceContext* context) : SimpleShaderHandler(device, context)
{
	// Ensure we set to zero to successfully trigger the input layout creation during LoadShader()
	this->inputLayout = 0;
	this->shader = 0;
	this->perInstanceCompatible = false;
}

SimpleVertexShader::SimpleVertexShader(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11InputLayout* inputLayout, bool perInstanceCompatible) : SimpleShaderHandler(device, context)
{
	// Save the custom input layout
	this->inputLayout = inputLayout;
	this->shader = 0;

	// Unable to determine from an input layout, require user to tell us
	this->perInstanceCompatible = perInstanceCompatible;
}

SimpleVertexShader::~SimpleVertexShader()
{
	CleanUp();
}

void SimpleVertexShader::SetShaderAndCBs()
{
	// Is shader valid?
	if (!isShaderValid) return;

	// Set the shader and input layout
	deviceContext->IASetInputLayout(inputLayout);
	deviceContext->VSSetShader(shader, 0, 0);

	// Set the constant buffers
	for (unsigned int i = 0; i < constantBufferCount; i++)
		deviceContext->VSSetConstantBuffers(constantBuffers[i].BindIndex, 1, &constantBuffers[i].constantBuffer);
}

void SimpleVertexShader::CleanUp()
{
	SimpleShaderHandler::CleanUp();

	if (shader) 
	{ 
		shader->Release(); 
		shader = 0; 
	}

	if (inputLayout) 
	{ 
		inputLayout->Release(); 
		inputLayout = 0; 
	}
}

bool SimpleVertexShader::CreateShader(ID3DBlob* shaderBlob)
{
	// Clean up first, in the event this function is called more than once on the same object.
	this->CleanUp();

	// Create the shader from the blob
	HRESULT result = device->CreateVertexShader(
		shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(),
		0,
		&shader);

	// Check if the shader was created successfully.
	if (result != S_OK)
		return false;

	// Do we already have an input layout?
	// (This would come from one of the constructor overloads)
	if (inputLayout)
		return true;

	ID3D11ShaderReflection* reflection;
	D3DReflect(
		shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(),
		IID_ID3D11ShaderReflection,
		(void**)&reflection);

	// Get shader info
	D3D11_SHADER_DESC shaderDesc;
	reflection->GetDesc(&shaderDesc);

	// Read input layout description from shader info
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
	for (unsigned int i = 0; i < shaderDesc.InputParameters; i++)
	{
		D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
		reflection->GetInputParameterDesc(i, &paramDesc);

		// Check the semantic name for "_PER_INSTANCE"
		std::string perInstanceStr = "_PER_INSTANCE";
		std::string semantic = paramDesc.SemanticName;
		int lenDiff = semantic.size() - perInstanceStr.size();
		bool isPerInstance = lenDiff >= 0 && semantic.compare(lenDiff, perInstanceStr.size(), perInstanceStr) == 0;

		// Fill out input element desc.
		D3D11_INPUT_ELEMENT_DESC elementDesc;
		elementDesc.SemanticName = paramDesc.SemanticName;
		elementDesc.SemanticIndex = paramDesc.SemanticIndex;
		elementDesc.InputSlot = 0;
		elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		elementDesc.InstanceDataStepRate = 0;

		// Replace anything affected by "per instance" data
		if (isPerInstance)
		{
			elementDesc.InputSlot = 1; // Assume per instance data comes from another input slot!
			elementDesc.InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
			elementDesc.InstanceDataStepRate = 1;

			perInstanceCompatible = true;
		}

		// Determine DXGI format
		if (paramDesc.Mask == 1)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
		}
		else if (paramDesc.Mask <= 3)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
		}
		else if (paramDesc.Mask <= 7)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		}
		else if (paramDesc.Mask <= 15)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		}

		// Save element desc
		inputLayoutDesc.push_back(elementDesc);
	}

	// Try to create Input Layout
	HRESULT hr = device->CreateInputLayout(
		&inputLayoutDesc[0],
		inputLayoutDesc.size(),
		shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(),
		&inputLayout);

	// All done, clean up
	reflection->Release();

	return true;
}

bool SimpleVertexShader::SetShaderResourceView(std::string name, ID3D11ShaderResourceView* srv)
{
	// Look for the variable and verify
	const SimpleSRV* srvInfo = GetShaderResourceViewInfo(name);
	if (srvInfo == 0)
		return false;

	// Set the shader resource view
	deviceContext->VSSetShaderResources(srvInfo->bindIndex, 1, &srv);

	// Success
	return true;
}

bool SimpleVertexShader::SetSamplerState(std::string name, ID3D11SamplerState* samplerState)
{
	// Look for the variable and verify
	const SimpleSampler* sampInfo = GetSamplerInfo(name);
	if (sampInfo == 0)
		return false;

	// Set the shader resource view
	deviceContext->VSSetSamplers(sampInfo->bindIndex, 1, &samplerState);

	// Success
	return true;
}

 
// *******************************************************
// SIMPLE PIXEL SHADER
// *******************************************************
SimplePixelShader::SimplePixelShader(ID3D11Device* device, ID3D11DeviceContext* context) : SimpleShaderHandler(device, context)
{
	this->shader = 0;
}

SimplePixelShader::~SimplePixelShader()
{
	CleanUp();
}

void SimplePixelShader::CleanUp()
{
	SimpleShaderHandler::CleanUp();
	if (shader) 
	{ 
		shader->Release(); 
		shader = 0; 
	}
}

bool SimplePixelShader::CreateShader(ID3DBlob* shaderBlob)
{
	// Clean up first, in the event this function is called more than once on the same object
	this->CleanUp();

	// Create the shader from the blob
	HRESULT result = device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), 0, &shader);

	// Check the result
	return (result == S_OK);
}

void SimplePixelShader::SetShaderAndCBs()
{
	// Is shader valid?
	if (!isShaderValid) return;

	// Set the shader
	deviceContext->PSSetShader(shader, 0, 0);

	// Set the constant buffers
	for (unsigned int i = 0; i < constantBufferCount; i++)
		deviceContext->PSSetConstantBuffers(constantBuffers[i].BindIndex, 1, &constantBuffers[i].constantBuffer);
}

bool SimplePixelShader::SetShaderResourceView(std::string name, ID3D11ShaderResourceView* srv)
{
	// Look for the variable and verify
	const SimpleSRV* srvInfo = GetShaderResourceViewInfo(name);
	if (srvInfo == 0)
		return false;

	// Set the shader resource view
	deviceContext->PSSetShaderResources(srvInfo->bindIndex, 1, &srv);

	// Success
	return true;
}

bool SimplePixelShader::SetSamplerState(std::string name, ID3D11SamplerState* samplerState)
{
	// Look for the variable and verify
	const SimpleSampler* sampInfo = GetSamplerInfo(name);
	if (sampInfo == 0)
		return false;

	// Set the shader resource view
	deviceContext->PSSetSamplers(sampInfo->bindIndex, 1, &samplerState);

	// Success
	return true;
}


