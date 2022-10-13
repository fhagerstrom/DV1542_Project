#include "Particles.h"

Particles::Particles()
{
}

Particles::~Particles()
{
}

bool Particles::Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
{
	HRESULT hr;

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

	InitializeBuffers(device);

	return true;
}

bool Particles::InitializeBuffers(ID3D11Device* device)
{
	unsigned long* indices;
	HRESULT hr;

	vertexCount = maxParticles * 6;
	indexCount = vertexCount;

	// Create the vertex array for the particles that will be rendered
	vertices = new Vertex[vertexCount];
	if (!vertices)
		return false;

	// Loop through all vertices of the particles and set values for them
	for (int i = 0; i < vertexCount; i++)
	{
		particles[i].position = { 0.0f, 30.0f, 0.0f };
		particles[i].seed = 100;
		particles[i].speed = 0.5f;
	}

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
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
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
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS;
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

	// Setup structured buffer description.
	// Bind it bitwise OR between SRV and UAV
	D3D11_BUFFER_DESC structuredDesc;
	structuredDesc.ByteWidth = vertexCount * sizeof(Particle);
	structuredDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	structuredDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	structuredDesc.StructureByteStride = sizeof(Particle);
	structuredDesc.Usage = D3D11_USAGE_DEFAULT;
	structuredDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA structuredData;
	ZeroMemory(&structuredData, sizeof(D3D11_SUBRESOURCE_DATA));
	structuredData.pSysMem = indices;
	structuredData.SysMemPitch = 0;
	structuredData.SysMemSlicePitch = 0;

	hr = device->CreateBuffer(&structuredDesc, &structuredData, &structuredBuffer);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Couldn't create structured buffer for particle system!", L"PARTICLES ERROR", MB_OK);
		return false;
	}

	// Setup a UAV desc for the compute shader side of the particle managing.
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.Format = DXGI_FORMAT_R32_FLOAT;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = particles->seed * 3;
	uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;

	hr = device->CreateUnorderedAccessView(vertexBuffer, &uavDesc, &particleUav);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Couldn't create UAV for particle system!", L"PARTICLES ERROR", MB_OK);
		return false;
	}



	return true;
}





