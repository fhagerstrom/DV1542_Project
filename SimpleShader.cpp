#include "SimpleShader.h"


// Constructor accepts DirectX device & context
SimpleShaderHandler::SimpleShaderHandler(ID3D11Device* device, ID3D11DeviceContext* context)
{
	// Save the device
	this->device = device;
	this->deviceContext = context;

	// Init fields
	constantBufferCount = 0;
	constantBuffers = 0;
	shaderBlob = 0;
}

// Destructor
SimpleShaderHandler::~SimpleShaderHandler()
{
	// Derived class destructors will call this class's CleanUp method
	if(shaderBlob)
		shaderBlob->Release();
}

// Cleans up the variable table and buffers - Some things will be handled by derived classes
void SimpleShaderHandler::CleanUp()
{
	// Handle constant buffers and local data buffers
	for (unsigned int i = 0; i < constantBufferCount; i++)
	{
		constantBuffers[i].ConstantBuffer->Release();
		delete[] constantBuffers[i].LocalDataBuffer;
	}

	if (constantBuffers)
	{
		delete[] constantBuffers;
		constantBufferCount = 0;
	}

	for (unsigned int i = 0; i < shaderResourceViews.size(); i++)
		delete shaderResourceViews[i];
	
	for (unsigned int i = 0; i < samplerStates.size(); i++)
		delete samplerStates[i];

	// Clean up tables
	varTable.clear();
	cbTable.clear();
	samplerTable.clear();
	textureTable.clear();
}

// Loads the specified shader and builds the variable table using shader
// reflection. This must be a separate step from the constructor since
// we can't invoke derived class overrides in the base class constructor.
//
// shaderFile - A wide string specifying the compiled shader to load
// 
// Returns true if shader is loaded properly, false otherwise
bool SimpleShaderHandler::LoadShaderFile(LPCWSTR shaderFile)
{
	// Load the shader to a blob and ensure it worked
	HRESULT hr = D3DReadFileToBlob(shaderFile, &shaderBlob);
	if (hr != S_OK)
	{
		//printf("ERROR! Could not load shader to a blob\n");
		return false;
	}

	// Create the shader - Calls an overloaded version of this abstract
	// method in the appropriate child class
	isShaderValid = CreateShader(shaderBlob);
	if (!isShaderValid)
	{
		//printf("ERROR! Could not create shader shaderblob\n");
		return false;
	}

	// Set up shader reflection to get information about
	// this shader and its variables,  buffers, etc.
	ID3D11ShaderReflection* reflection;
	D3DReflect(
		shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(),
		IID_ID3D11ShaderReflection,
		(void**)&reflection);
	
	// Get the description of the shader
	D3D11_SHADER_DESC shaderDesc;
	reflection->GetDesc(&shaderDesc);

	// Create resource arrays
	constantBufferCount = shaderDesc.ConstantBuffers;
	constantBuffers = new SimpleConstantBuffer[constantBufferCount];
	
	// Handle bound resources (like shaders and samplers)
	unsigned int resourceCount = shaderDesc.BoundResources;
	for (unsigned int r = 0; r < resourceCount; r++)
	{
		// Get this resource's description
		D3D11_SHADER_INPUT_BIND_DESC resourceDesc;
		reflection->GetResourceBindingDesc(r, &resourceDesc);

		// Check the type
		switch (resourceDesc.Type)
		{
			case D3D_SIT_TEXTURE: // A texture resource
			{
				// Create the SRV wrapper
				SimpleSRV* srv = new SimpleSRV();
				srv->bindIndex = resourceDesc.BindPoint;	// Shader bind point
				srv->index = shaderResourceViews.size();	// Raw index

				textureTable.insert(std::pair<std::string, SimpleSRV*>(resourceDesc.Name, srv));
				shaderResourceViews.push_back(srv);
			}
				break;

			case D3D_SIT_SAMPLER: // A sampler resource
			{
				// Create the sampler wrapper
				SimpleSampler* samp = new SimpleSampler();
				samp->bindIndex = resourceDesc.BindPoint;	// Shader bind point
				samp->index = samplerStates.size();			// Raw index

				samplerTable.insert(std::pair<std::string, SimpleSampler*>(resourceDesc.Name, samp));
				samplerStates.push_back(samp);
			}
				break;
		}
	}

	// Loop through all constant buffers
	for (unsigned int b = 0; b < constantBufferCount; b++)
	{
		// Get this buffer
		ID3D11ShaderReflectionConstantBuffer* cb =
			reflection->GetConstantBufferByIndex(b);
		
		// Get the description of this buffer
		D3D11_SHADER_BUFFER_DESC bufferDesc;
		cb->GetDesc(&bufferDesc);
		
		// Get the description of the resource binding, so
		// we know exactly how it's bound in the shader
		D3D11_SHADER_INPUT_BIND_DESC bindDesc;
		reflection->GetResourceBindingDescByName(bufferDesc.Name, &bindDesc);
		
		// Set up the buffer and put its pointer in the table
		constantBuffers[b].bindIndex = bindDesc.BindPoint;
		constantBuffers[b].Name = bufferDesc.Name;
		cbTable.insert(std::pair<std::string, SimpleConstantBuffer*>(bufferDesc.Name, &constantBuffers[b]));

		// Create this constant buffer
		D3D11_BUFFER_DESC newBuffDesc;
		newBuffDesc.Usage = D3D11_USAGE_DEFAULT;
		newBuffDesc.ByteWidth = bufferDesc.Size;
		newBuffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		newBuffDesc.CPUAccessFlags = 0;
		newBuffDesc.MiscFlags = 0;
		newBuffDesc.StructureByteStride = 0;
		device->CreateBuffer(&newBuffDesc, 0, &constantBuffers[b].ConstantBuffer);

		// Set up the data buffer for this constant buffer
		constantBuffers[b].Size = bufferDesc.Size;
		constantBuffers[b].LocalDataBuffer = new unsigned char[bufferDesc.Size];
		ZeroMemory(constantBuffers[b].LocalDataBuffer, bufferDesc.Size);

		// Loop through all variables in this buffer
		for (unsigned int v = 0; v < bufferDesc.Variables; v++)
		{
			// Get this variable
			ID3D11ShaderReflectionVariable* var =
				cb->GetVariableByIndex(v);
			
			// Get the description of the variable and its type
			D3D11_SHADER_VARIABLE_DESC varDesc;
			var->GetDesc(&varDesc);

			// Create the variable struct
			SimpleShaderVariable varStruct;
			varStruct.ConstantBufferIndex = b;
			varStruct.ByteOffset = varDesc.StartOffset;
			varStruct.Size = varDesc.Size;
			
			// Get a string version
			std::string varName(varDesc.Name);

			// Add this variable to the table and the constant buffer
			varTable.insert(std::pair<std::string, SimpleShaderVariable>(varName, varStruct));
			constantBuffers[b].Variables.push_back(varStruct);
		}
	}

	// All set
	reflection->Release();
	return true;
}

// Helper for looking up a variable by name and also
// verifying that it is the requested size
// 
// name - the name of the variable to look for
// size - the size of the variable (for verification), or -1 to bypass
SimpleShaderVariable* SimpleShaderHandler::FindVariable(std::string name, int size)
{
	// Look for the key
	std::unordered_map<std::string, SimpleShaderVariable>::iterator result =
		varTable.find(name);

	// Did we find the key?
	if (result == varTable.end())
	{
		//printf("ERROR! SimpleShaderHandler::FindVariable failed!\n");
		return 0;
	}
	// Grab the result from the iterator
	SimpleShaderVariable* var = &(result->second);

	// Is the data size correct ?
	if (size > 0 && var->Size != size)
	{
		//printf("ERROR! Data size is incorrect!\n");
		return 0;
	}

	// Success
	return var;
}

// Helper for looking up a constant buffer by name
SimpleConstantBuffer* SimpleShaderHandler::FindConstantBuffer(std::string name)
{
	// Look for the key
	std::unordered_map<std::string, SimpleConstantBuffer*>::iterator result =
		cbTable.find(name);

	// Did we find the key?
	if (result == cbTable.end())
	{
		//printf("ERROR! couldn't find ConstantBuffer! \n");
		return 0;
	}
	// Success
	return result->second;
}

// Sets the shader and associated constant buffers.
void SimpleShaderHandler::SetShader()
{
	// Ensure the shader is valid
	if (!isShaderValid) 
	{	
		//printf("ERROR! Could not ensure that simpleshader is valid!\n");
		return;
	}
	// Set the shader and any relevant constant buffers, which
	// is an overloaded method in a subclass
	SetShaderAndCBs();
}

// Copies the relevant data to the all of this 
// shader's constant buffers.  To just copy one
// buffer, use CopyBufferData()
void SimpleShaderHandler::CopyAllBufferData()
{
	// Ensure the shader is valid
	if (!isShaderValid)
	{
		//printf("ERROR! Could not ensure that shader is valid\n");
		return;
	}
	// Loop through the constant buffers and copy all data
	for (unsigned int i = 0; i < constantBufferCount; i++)
	{
		// Copy the entire local data buffer
		deviceContext->UpdateSubresource(
			constantBuffers[i].ConstantBuffer, 0, 0,
			constantBuffers[i].LocalDataBuffer, 0, 0);
	}
}

// Copies local data to the shader's specified constant buffer
//
// index - The index of the buffer to copy.
//         Useful for updating more frequently-changing
//         variables without having to re-copy all buffers.
//  
// The "index" of the buffer might NOT be the same as its register, especially if the buffers are bound to non-sequential registers.
void SimpleShaderHandler::CopyBufferData(unsigned int index)
{
	// Ensure the shader is valid
	if (!isShaderValid)
	{
		//printf("ERROR! Could not ensure that shader is valid Copybufferdata!\n");
		return;
	}
	// Validate the index
	if (index >= this->constantBufferCount)
	{
		//printf("ERROR! Could not validate index! constantBufferCount\n");
		return;
	}
	// Check for the buffer
	SimpleConstantBuffer* cb = &this->constantBuffers[index];
	if (!cb)
	{
		//printf("ERROR! Could not check constant buffer properly!\n");
		return;
	}
	// Copy the data and get out
	deviceContext->UpdateSubresource(cb->ConstantBuffer, 0, 0, cb->LocalDataBuffer, 0, 0);
}

// Copies local data to the shader's specified constant buffer
// bufferName - Specifies the name of the buffer to copy.
//              Useful for updating more frequently-changing
//              variables without having to re-copy all buffers.
void SimpleShaderHandler::CopyBufferData(std::string bufferName)
{
	// Ensure the shader is valid
	if (!isShaderValid)
	{
		//printf("ERROR! Shader not valid! CopyBufferData \n");
		return;
	}
	// Check for the buffer
	SimpleConstantBuffer* cb = this->FindConstantBuffer(bufferName);
	if (!cb)
	{
		//printf("ERROR! Could not check for the buffer properly! CopyBufferData \n");
		return;
	}
	// Copy the data and get out
	deviceContext->UpdateSubresource(
		cb->ConstantBuffer, 0, 0, 
		cb->LocalDataBuffer, 0, 0);
}

// Sets a variable by name with arbitrary data of the specified size
//
// name - The name of the shader variable
// data - The data to set in the buffer
// size - The size of the data (this must match the variable's size)
//
// Returns true if data is copied, false if variable doesn't exist or sizes don't match
bool SimpleShaderHandler::SetData(std::string name, const void* data, unsigned int size)
{
	// Look for the variable and verify
	SimpleShaderVariable* var = FindVariable(name, size);
	if (var == 0)
	{
		//printf("ERROR! Could not verify variable! SetData\n");
		return false;
	}
	// Set the data in the local data buffer
	memcpy(
		constantBuffers[var->ConstantBufferIndex].LocalDataBuffer + var->ByteOffset,
		data,
		size);

	// Success
	return true;
}

// Sets INTEGER data
bool SimpleShaderHandler::SetInt(std::string name, int data)
{
	return this->SetData(name, (void*)(&data), sizeof(int));
}

// Sets a FLOAT variable by name in the local data buffer
bool SimpleShaderHandler::SetFloat(std::string name, float data)
{
	return this->SetData(name, (void*)(&data), sizeof(float));
}

// Sets a FLOAT2 variable by name in the local data buffer
bool SimpleShaderHandler::SetFloat2(std::string name, const float data[2])
{
	return this->SetData(name, (void*)data, sizeof(float) * 2);
}

// Sets a FLOAT2 variable by name in the local data buffer
bool SimpleShaderHandler::SetFloat2(std::string name, const DirectX::XMFLOAT2 data)
{
	return this->SetData(name, &data, sizeof(float) * 2);
}

// Sets a FLOAT3 variable by name in the local data buffer
bool SimpleShaderHandler::SetFloat3(std::string name, const float data[3])
{
	return this->SetData(name, (void*)data, sizeof(float) * 3);
}

// Sets a FLOAT3 variable by name in the local data buffer
bool SimpleShaderHandler::SetFloat3(std::string name, const DirectX::XMFLOAT3 data)
{
	return this->SetData(name, &data, sizeof(float) * 3);
}

// Sets a FLOAT4 variable by name in the local data buffer
bool SimpleShaderHandler::SetFloat4(std::string name, const float data[4])
{
	return this->SetData(name, (void*)data, sizeof(float) * 4);
}

// Sets a FLOAT4 variable by name in the local data buffer
bool SimpleShaderHandler::SetFloat4(std::string name, const DirectX::XMFLOAT4 data)
{
	return this->SetData(name, &data, sizeof(float) * 4);
}

// Sets a MATRIX (4x4) variable by name in the local data buffer
bool SimpleShaderHandler::SetMatrix4x4(std::string name, const float data[16])
{
	return this->SetData(name, (void*)data, sizeof(float) * 16);
}

// Sets a MATRIX (4x4) variable by name in the local data buffer
bool SimpleShaderHandler::SetMatrix4x4(std::string name, const DirectX::XMFLOAT4X4 data)
{
	return this->SetData(name, &data, sizeof(float) * 16);
}

// Gets info about a shader variable, if it exists
const SimpleShaderVariable* SimpleShaderHandler::GetVariableInfo(std::string name)
{
	return FindVariable(name, -1);
}

// Gets info about an SRV in the shader (or null)
//
// name - the name of the SRV
const SimpleSRV* SimpleShaderHandler::GetShaderResourceViewInfo(std::string name)
{
	// Look for the key
	std::unordered_map<std::string, SimpleSRV*>::iterator result =
		textureTable.find(name);

	// Did we find the key?
	if (result == textureTable.end())
	{
		//printf("ERROR! ShaderResourceViewInfo could not be found! \n");
		return 0;
	}
	// Success
	return result->second;
}

// Gets info about an SRV in the shader (or null)
//
// index - the index of the SRV
const SimpleSRV* SimpleShaderHandler::GetShaderResourceViewInfo(unsigned int index)
{
	// Valid index?
	if (index >= shaderResourceViews.size())
	{
		//printf("ERROR! No valid index \n");
		return 0;
	}
	// Grab the bind index
	return shaderResourceViews[index];
}

// Gets info about a sampler in the shader (or null)
// 
// name - the name of the sampler
const SimpleSampler* SimpleShaderHandler::GetSamplerInfo(std::string name)
{
	// Look for the key
	std::unordered_map<std::string, SimpleSampler*>::iterator result =
		samplerTable.find(name);

	// Did we find the key?
	if (result == samplerTable.end())
	{
		//printf("ERROR! Sampler info could not be found. \n");
		return 0;
	}
	// Success
	return result->second;
}

// Gets info about a sampler in the shader (or null)
// 
// index - the index of the sampler
const SimpleSampler* SimpleShaderHandler::GetSamplerInfo(unsigned int index)
{
	// Valid index?
	if (index >= samplerStates.size())
	{
		//printf("ERROR! Could not get Sampler info!\n");
		return 0;
	}
	// Grab the bind index
	return samplerStates[index];
}

// Gets the number of constant buffers in this shader
unsigned int SimpleShaderHandler::GetBufferCount() { return constantBufferCount; }

// Gets the size of a particular constant buffer, or -1
unsigned int SimpleShaderHandler::GetBufferSize(unsigned int index)
{
	// Valid index?
	if (index >= constantBufferCount)
	{
		printf("ERROR! GetBufferSize index invalid!\n");
		return -1;
	}
	// Grab the size
	return constantBuffers[index].Size;
}

// Gets info about a particular constant buffer 
// by name, if it exists
const SimpleConstantBuffer * SimpleShaderHandler::GetBufferInfo(std::string name)
{
	return FindConstantBuffer(name);
}

// Gets info about a particular constant buffer 
//
// index - the index of the constant buffer
const SimpleConstantBuffer * SimpleShaderHandler::GetBufferInfo(unsigned int index)
{
	// Check for valid index
	if (index >= constantBufferCount)
	{
		//printf("ERROR! GetBufferInfo Index was not valid! \n");
		return 0;
	}
	// Return the specific buffer
	return &constantBuffers[index];
}

//*****************************************************************************
// VERTEX SHADER
//*****************************************************************************

// Constructor just calls the base
SimpleVertexShader::SimpleVertexShader(ID3D11Device* device, ID3D11DeviceContext* context) : SimpleShaderHandler(device, context) 
{ 
	// Ensure we set to zero to successfully trigger the input layout creation during LoadShader()
	this->inputLayout = 0;
	this->shader = 0;
	this->perInstanceCompatible = false;
}

// Constructor overload which takes a custom input layout
//
// Passing in a valid input layout will stop LoadShader()
// from creating an input layout from shader reflection
SimpleVertexShader::SimpleVertexShader(ID3D11Device * device, ID3D11DeviceContext * context, ID3D11InputLayout * inputLayout, bool perInstanceCompatible) : SimpleShaderHandler(device, context)
{
	// Save input layout.
	this->inputLayout = inputLayout;
	this->shader = 0;

	// Unable to determine from an input layout, require user to tell us
	this->perInstanceCompatible = perInstanceCompatible;
}

// Destructor - Clean up shader (base will be called automatically)
SimpleVertexShader::~SimpleVertexShader()
{
	CleanUp();
}

// Handles cleaning up shader and base class clean up
void SimpleVertexShader::CleanUp()
{
	SimpleShaderHandler::CleanUp();
	if (shader) { shader->Release(); shader = 0; }
	if (inputLayout) { inputLayout->Release(); inputLayout = 0; }
}

// Creates a vertex shader
// shaderBlob - The shader's compiled code
// Returns true if shader is created correctly, false otherwise
bool SimpleVertexShader::CreateShader(ID3DBlob* shaderBlob)
{
	// Clean up
	this->CleanUp();

	// Create the shader from the blob
	HRESULT result = device->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), 0, &shader);

	// Did the creation work?
	if (result != S_OK)
	{
		//printf("ERROR! The creation of VertexShader did not work! \n");
		return false;
	}
	// Do we already have an input layout? This would come from one of the constructor overloads
	if (inputLayout)
	{
		return true;
	}
	// Vertex shader was created successfully, so we now use the
	// shader code to re-reflect and create an input layout that 
	// matches what the vertex shader expects.
	// 
	// Reflect shader info
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
	for (unsigned int i = 0; i< shaderDesc.InputParameters; i++)
	{
		D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
		reflection->GetInputParameterDesc(i, &paramDesc);

		// Check the semantic name for "_PER_INSTANCE"
		std::string perInstanceStr = "_PER_INSTANCE";
		std::string sem = paramDesc.SemanticName;
		int lenDiff = sem.size() - perInstanceStr.size();
		bool isPerInstance = 
			lenDiff >= 0 &&
			sem.compare(lenDiff, perInstanceStr.size(), perInstanceStr) == 0;

		// Fill out input element desc.
		D3D11_INPUT_ELEMENT_DESC elementDesc;
		elementDesc.SemanticName = paramDesc.SemanticName;
		elementDesc.SemanticIndex = paramDesc.SemanticIndex;
		elementDesc.InputSlot = 0;
		elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		elementDesc.InstanceDataStepRate = 0;

		// Replace anything affected by "per instance" data.
		if (isPerInstance)
		{
			elementDesc.InputSlot = 1; // Assume per instance data comes from another input slot.
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
	HRESULT hr = device->CreateInputLayout(&inputLayoutDesc[0], inputLayoutDesc.size(), shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), &inputLayout);

	// All done, clean up.
	reflection->Release();
	return true;
}

// Sets the vertex shader, input layout and constant buffers for drawing.
void SimpleVertexShader::SetShaderAndCBs()
{
	// Is shader valid?
	if (!isShaderValid)
	{
		//printf("ERROR! SetShaderAndCBs:: shader is not valid.\n");
		return;
	}
	// Set the shader and input layout
	deviceContext->IASetInputLayout(inputLayout);
	deviceContext->VSSetShader(shader, 0, 0);

	// Set the constant buffers
	for (unsigned int i = 0; i < constantBufferCount; i++)
	{
		deviceContext->VSSetConstantBuffers(constantBuffers[i].bindIndex, 1, &constantBuffers[i].ConstantBuffer);
	}
}

// Sets a shader resource view in the vertex shader stage
//
// name - The name of the texture resource in the shader
// srv - The shader resource view of the texture in GPU memory
//
// Returns true if a texture of the given name was found, false otherwise
bool SimpleVertexShader::SetShaderResourceView(std::string name, ID3D11ShaderResourceView* srv)
{
	// Look for the variable and verify
	const SimpleSRV* srvInfo = GetShaderResourceViewInfo(name);
	if (srvInfo == 0)
	{
		//printf("ERROR! SetShaderResourceView::Couldn't verify the variable! \n");
		return false;
	}
	// Set the shader resource view
	deviceContext->VSSetShaderResources(srvInfo->bindIndex, 1, &srv);

	// Success
	return true;
}

// Sets a sampler state in the vertex shader stage
//
// name - The name of the sampler state in the shader
// samplerState - The sampler state in GPU memory
bool SimpleVertexShader::SetSamplerState(std::string name, ID3D11SamplerState* samplerState)
{
	// Look for the variable and verify
	const SimpleSampler* sampInfo = GetSamplerInfo(name);
	if (sampInfo == 0)
	{
		//printf("ERROR! Couldn't Set SamplerState \n");
		return false;
	}
	// Set the shader resource view
	deviceContext->VSSetSamplers(sampInfo->bindIndex, 1, &samplerState);

	// Success
	return true;
}

//*****************************************************************************
// PIXEL SHADER
//*****************************************************************************

// Constructor just calls the base
SimplePixelShader::SimplePixelShader(ID3D11Device* device, ID3D11DeviceContext* context)
	: SimpleShaderHandler(device, context) 
{ 
	this->shader = 0;
}

// Destructor - Clean up shader (base will be called automatically)

SimplePixelShader::~SimplePixelShader()
{
	CleanUp();
}

// Handles cleaning up shader and base class clean up
void SimplePixelShader::CleanUp()
{
	SimpleShaderHandler::CleanUp();
	if (shader) { shader->Release(); shader = 0; }
}

// Creates the pixel shader
//
// shaderBlob - The shader's compiled code
//
// Returns true if shader is created correctly, false otherwise
bool SimplePixelShader::CreateShader(ID3DBlob* shaderBlob)
{
	// Clean up first, in the event this method is
	// called more than once on the same object
	this->CleanUp();

	// Create the shader from the blob
	HRESULT result = device->CreatePixelShader(
		shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(),
		0,
		&shader);

	// Check the result
	return (result == S_OK);
}

// Sets the pixel shader and constant buffers.
void SimplePixelShader::SetShaderAndCBs()
{
	// Is shader valid?
	if (!isShaderValid)
	{
		//printf("ERROR! SetShaderAndCBs:: shader is invalid! \n");
		return;
	}
	// Set the shader
	deviceContext->PSSetShader(shader, 0, 0);

	// Set the constant buffers
	for (unsigned int i = 0; i < constantBufferCount; i++)
	{
		deviceContext->PSSetConstantBuffers(
			constantBuffers[i].bindIndex,
			1,
			&constantBuffers[i].ConstantBuffer);
	}
}

// Sets a shader resource view in the pixel shader stage
//
// name - The name of the texture resource in the shader
// srv - The shader resource view of the texture in GPU memory
//
// Returns true if a texture of the given name was found, false otherwise
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

// Sets a sampler state in the pixel shader stage
//
// name - The name of the sampler state in the shader
// samplerState - The sampler state in GPU memory
//
// Returns true if a sampler of the given name was found, false otherwise
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

///////////////////////////////////////////////////////////////////////////////
// ------ SIMPLE GEOMETRY SHADER ----------------------------------------------
///////////////////////////////////////////////////////////////////////////////

// --------------------------------------------------------
// Constructor calls the base and sets up potential stream-out options
// --------------------------------------------------------
SimpleGeometryShader::SimpleGeometryShader(ID3D11Device* device, ID3D11DeviceContext* context, bool useStreamOut, bool allowStreamOutRasterization)
	: SimpleShaderHandler(device, context)
{
	this->shader = 0;
	this->useStreamOut = useStreamOut;
	this->allowStreamOutRasterization = allowStreamOutRasterization;
}

// --------------------------------------------------------
// Destructor - Clean up actual shader (base will be called automatically)
// --------------------------------------------------------
SimpleGeometryShader::~SimpleGeometryShader()
{
	CleanUp();
}

// --------------------------------------------------------
// Handles cleaning up shader and base class clean up
// --------------------------------------------------------
void SimpleGeometryShader::CleanUp()
{
	SimpleShaderHandler::CleanUp();
	if (shader) { shader->Release(); shader = 0; }
}

// --------------------------------------------------------
// Creates the DirectX Geometry shader
//
// shaderBlob - The shader's compiled code
//
// Returns true if shader is created correctly, false otherwise
// --------------------------------------------------------
bool SimpleGeometryShader::CreateShader(ID3DBlob* shaderBlob)
{
	// Clean up first, in the event this method is
	// called more than once on the same object
	this->CleanUp();

	// Using stream out?
	if (useStreamOut)
		return this->CreateShaderWithStreamOut(shaderBlob);

	// Create the shader from the blob
	HRESULT result = device->CreateGeometryShader(
		shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(),
		0,
		&shader);

	// Check the result
	return (result == S_OK);
}

// --------------------------------------------------------
// Creates the DirectX Geometry shader and sets it up for
// stream output, if possible.
//
// shaderBlob - The shader's compiled code
//
// Returns true if shader is created correctly, false otherwise
// --------------------------------------------------------
bool SimpleGeometryShader::CreateShaderWithStreamOut(ID3DBlob* shaderBlob)
{
	// Clean up first, in the event this method is
	// called more than once on the same object
	this->CleanUp();

	// Reflect shader info
	ID3D11ShaderReflection* refl;
	D3DReflect(
		shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(),
		IID_ID3D11ShaderReflection,
		(void**)&refl);

	// Get shader info
	D3D11_SHADER_DESC shaderDesc;
	refl->GetDesc(&shaderDesc);

	// Set up the output signature
	streamOutVertexSize = 0;
	std::vector<D3D11_SO_DECLARATION_ENTRY> soDecl;
	for (unsigned int i = 0; i < shaderDesc.OutputParameters; i++)
	{
		// Get the info about this entry
		D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
		refl->GetOutputParameterDesc(i, &paramDesc);

		// Create the SO Declaration
		D3D11_SO_DECLARATION_ENTRY entry;
		entry.SemanticIndex = paramDesc.SemanticIndex;
		entry.SemanticName = paramDesc.SemanticName;
		entry.Stream = paramDesc.Stream;
		entry.StartComponent = 0; // Assume starting at 0
		entry.OutputSlot = 0; // Assume the first output slot

		// Check the mask to determine how many components are used
		entry.ComponentCount = CalcComponentCount(paramDesc.Mask);

		// Increment the size
		streamOutVertexSize += entry.ComponentCount * sizeof(float);

		// Add to the declaration
		soDecl.push_back(entry);
	}

	// Rasterization allowed?
	unsigned int rast = allowStreamOutRasterization ? 0 : D3D11_SO_NO_RASTERIZED_STREAM;

	// Create the shader
	HRESULT result = device->CreateGeometryShaderWithStreamOutput(
		shaderBlob->GetBufferPointer(), // Shader blob pointer
		shaderBlob->GetBufferSize(),    // Shader blob size
		&soDecl[0],                     // Stream out declaration
		soDecl.size(),                  // Number of declaration entries
		NULL,                           // Buffer strides (not used - assume tightly packed?)
		0,                              // No buffer strides
		rast,                           // Index of the stream to rasterize (if any)
		NULL,                           // Not using class linkage
		&shader);

	return (result == S_OK);
}

// --------------------------------------------------------
// Creates a vertex buffer that is compatible with the stream output
// delcaration that was used to create the shader.  This buffer will
// not be cleaned up (Released) by the simple shader - Immediately returns
// false if the shader was not created with stream output, the shader
// isn't valid or the determined stream out vertex size is zero.
//
// buffer - Pointer to an ID3D11Buffer pointer to hold the buffer ref
// vertexCount - Amount of vertices the buffer should hold
//
// Returns true if buffer is created successfully AND stream output
// was used to create the shader. False otherwise.
// NEEDS TO BE MANUALLY CLEANED!
// --------------------------------------------------------
bool SimpleGeometryShader::CreateCompatibleStreamOutBuffer(ID3D11Buffer** buffer, int vertexCount)
{
	// Was stream output actually used?
	if (!this->useStreamOut || !isShaderValid || streamOutVertexSize == 0)
		return false;

	// Set up the buffer description
	D3D11_BUFFER_DESC desc;
	desc.BindFlags = D3D11_BIND_STREAM_OUTPUT | D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth = streamOutVertexSize * vertexCount;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;

	// Attempt to create the buffer and return the result
	HRESULT result = device->CreateBuffer(&desc, 0, buffer);
	return (result == S_OK);
}

// --------------------------------------------------------
// Helper method to unbind all stream out buffers from the SO stage
// --------------------------------------------------------
void SimpleGeometryShader::UnbindStreamOutStage(ID3D11DeviceContext* deviceContext)
{
	unsigned int offset = 0;
	ID3D11Buffer* unset[1] = { 0 };
	deviceContext->SOSetTargets(1, unset, &offset);
}

// --------------------------------------------------------
// Sets the geometry shader and constant buffers for
// future DirectX drawing
// --------------------------------------------------------
void SimpleGeometryShader::SetShaderAndCBs()
{
	// Is shader valid?
	if (!isShaderValid) return;

	// Set the shader
	deviceContext->GSSetShader(shader, 0, 0);

	// Set the constant buffers?
	for (unsigned int i = 0; i < constantBufferCount; i++)
	{
		deviceContext->GSSetConstantBuffers(
			constantBuffers[i].bindIndex,
			1,
			&constantBuffers[i].ConstantBuffer);
	}
}

// --------------------------------------------------------
// Sets a shader resource view in the Geometry shader stage
//
// name - The name of the texture resource in the shader
// srv - The shader resource view of the texture in GPU memory
//
// Returns true if a texture of the given name was found, false otherwise
// --------------------------------------------------------
bool SimpleGeometryShader::SetShaderResourceView(std::string name, ID3D11ShaderResourceView* srv)
{
	// Look for the variable and verify
	const SimpleSRV* srvInfo = GetShaderResourceViewInfo(name);
	if (srvInfo == 0)
		return false;

	// Set the shader resource view
	deviceContext->GSSetShaderResources(srvInfo->bindIndex, 1, &srv);

	// Success
	return true;
}

// --------------------------------------------------------
// Sets a sampler state in the Geometry shader stage
//
// name - The name of the sampler state in the shader
// samplerState - The sampler state in GPU memory
//
// Returns true if a sampler of the given name was found, false otherwise
// --------------------------------------------------------
bool SimpleGeometryShader::SetSamplerState(std::string name, ID3D11SamplerState* samplerState)
{
	// Look for the variable and verify
	const SimpleSampler* sampInfo = GetSamplerInfo(name);
	if (sampInfo == 0)
		return false;

	// Set the shader resource view
	deviceContext->GSSetSamplers(sampInfo->bindIndex, 1, &samplerState);

	// Success
	return true;
}

// --------------------------------------------------------
// Calculates the number of components specified by a parameter description mask
//
// mask - The mask to check (only values 0 - 15 are considered)
//
// Returns an integer between 0 - 4 inclusive
// --------------------------------------------------------
unsigned int SimpleGeometryShader::CalcComponentCount(unsigned int mask)
{
	unsigned int result = 0;
	result += (unsigned int)((mask & 1) == 1);
	result += (unsigned int)((mask & 2) == 2);
	result += (unsigned int)((mask & 4) == 4);
	result += (unsigned int)((mask & 8) == 8);
	return result;
}