#include "Particles.h"

Particles::Particles()
{
}

Particles::~Particles()
{
}

bool Particles::Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
{
	bool result;

	// Set deviation on particles. This is a point from where the particle will "spawn" and then be randomized from.
	deviationX = 30.0f;
	deviationY = 0.2f;
	deviationZ = 30.0f;

	particleSpeed = 15.0f;
	particleSpeedVariation = 0.5f;

	particleSize = 0.5f;

	maxParticles = 100;

	// Set accumulated time to 0. 
	// This is from the beginning and the time will increment based on delta time.
	accumulatedTime = 0.0f;

	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	return true;
}

bool Particles::Update(float frameTime, ID3D11DeviceContext* context)
{
	bool result;

	// Update behaviors etc...


	// Call UpdateBuffers
	result = UpdateBuffers(context);
	if (!result)
	{
		return false;
	}


	return true;
}

void Particles::Render(ID3D11DeviceContext* context)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(Vertex);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer.
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
}

void Particles::Shutdown()
{
}

bool Particles::InitializeBuffers(ID3D11Device* device)
{
	HRESULT hr;

	vertexCount = maxParticles * 6;
	indexCount = vertexCount;

	// Create the vertex array for the particles that will be rendered
	vertices = new Vertex[vertexCount];
	if (!vertices)
		return false;

	// Loop through all vertices of the particles and set values for them


	// Create the index array.
	indices = new unsigned long[indexCount];
	if (!indices)
		return false;

	for (int i = 0; i < indexCount; i++) 
	{
		indices[i] = i;
	}

	// Set up the description of the dynamic vertex buffer.
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	D3D11_SUBRESOURCE_DATA vertexData;
	ZeroMemory(&vertexData, sizeof(D3D11_SUBRESOURCE_DATA));
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now finally create the vertex buffer.
	hr = device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
	if (FAILED(hr))
		return false;


	// Set up the description of the static index buffer
	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data
	D3D11_SUBRESOURCE_DATA indexData;
	ZeroMemory(&indexData, sizeof(D3D11_SUBRESOURCE_DATA));
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	hr = device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);
	if (FAILED(hr))
		return false;

	// Release the index array since it is no longer needed.
	delete[] indices;
	indices = 0;

	// Setup a UAV desc for the compute shader side of the particle managing.
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.Format = DXGI_FORMAT_R32_FLOAT;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = maxParticles * 3;
	uavDesc.Buffer.Flags = 0;

	hr = device->CreateUnorderedAccessView(vertexBuffer, &uavDesc, &particleUav);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Couldn't create UAV for particle system!", L"PARTICLES ERROR", MB_OK);
		return false;
	}

	return true;
}

bool Particles::UpdateBuffers(ID3D11DeviceContext* context)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	Vertex* vertexPtr;
	// ID3D11Resource* particleResource;

	// particleUav->GetResource(&particleResource);

	// context->CopyResource(vertexBuffer, particleResource);
	/*
		Resource is mapped for writing, the previous contents of the resource will be undefined.
		Map/Unmap is a common use with temporary buffers. - Dynamic usage of our vertexbuffer. (It is at least prefered)
	*/

	// Lock the vertex buffer
	//hr = context->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);	// maybe D3D11_MAP_WRITE_NO_OVERWRITE
	//if (FAILED(hr))
	//{
	//	return false;
	//}

	//// Get a pointer to the data in the vertex buffer
	//vertexPtr = (Vertex*)mappedResource.pData;

	//// Copy the data into the vertex buffer
	//memcpy(vertexPtr, (void*)vertices, (sizeof(Vertex) * vertexCount));

	//// Unlock the vertex buffer
	//context->Unmap(vertexBuffer, 0);
	return true;
}

void Particles::CreateParticle(float frameTime)
{
	// Increment the frame time.
	accumulatedTime += frameTime;

	// Set emit particle to false for now.
	bool emitParticle = false;

	// Check if it is time to emit a new particle or not.
	if (accumulatedTime > 0.1f) 
	{
		accumulatedTime = 0.0f;
		emitParticle = true;
	}
}

void Particles::UpdateParticle(float frameTime)
{
}

void Particles::DeleteParticles()
{
}

bool Particles::LoadTexture(ID3D11Device* device, LPCWSTR textureFile)
{
	bool result;

	texture = new Texture;
	if (!texture)
		return false;

	result = texture->Initialize(device, textureFile);
	if (!result)
		return false;
	return true;
}





