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
	HRESULT result;

	vertexCount = maxParticles * 6;
	indexCount = vertexCount;

	// Create the vertex array for the particles that will be rendered.
	vertices = new Vertex[vertexCount];
	if (!vertices)
		return false;

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
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
	if (FAILED(result))
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
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);
	if (FAILED(result))
		return false;

	// Release the index array since it is no longer needed.
	delete[] indices;
	indices = 0;

	// Setup a UAV desc for the compute shader side of the particle managing.
	D3D11_UNORDERED_ACCESS_VIEW_DESC UavDesc;
	UavDesc.Format = DXGI_FORMAT_R32_FLOAT;
	UavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	UavDesc.Buffer.FirstElement = 0;
	UavDesc.Buffer.NumElements = maxParticles * 3;
	UavDesc.Buffer.Flags = 0;

	device->CreateUnorderedAccessView(vertexBuffer, &UavDesc, &particleUav);

	return true;
}

void Particles::CreateParticle(float frameTime)
{

}




