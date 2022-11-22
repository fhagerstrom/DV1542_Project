#include "Particles.h"

Particles::Particles()
{
}

Particles::~Particles()
{
}

bool Particles::Initialize(ID3D11Device* device, ID3D11DeviceContext* context, int maxParticles)
{
	bool result;



	result = InitializeBuffers(device, maxParticles);
	if (!result)
	{
		return false;
	}

	return true;
}

bool Particles::Update(float deltaTime)
{
	bool result;




	//// Call UpdateBuffers
	//result = UpdateBuffers(context);
	//if (!result)
	//{
	//	return false;
	//}


	return true;
}

void Particles::Render(ID3D11DeviceContext* context, int maxParticles)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	/*stride = sizeof(Vertex);
	offset = 0;*/

	// Set a nulled input layout for particles since we perform vertex pulling instead of using vertex / index buffers.
	context->IASetInputLayout(NULL);

	// Set the type of primitive that should be rendered for particles.
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	context->Draw(maxParticles, 0);

}

void Particles::Shutdown()
{
	delete camera;
}

bool Particles::InitializeBuffers(ID3D11Device* device, int maxParticles)
{
	HRESULT hr;

	/* OLD CODE, KEEP AS BACKUP */
	//vertexCount = maxParticles * 6;
	//indexCount = vertexCount;

	//// Create the vertex array for the particles that will be rendered
	//vertices = new Vertex[vertexCount];
	//if (!vertices)
	//	return false;

	//// Create the index array.
	//indices = new unsigned long[indexCount];
	//if (!indices)
	//	return false;

	//for (int i = 0; i < indexCount; i++) 
	//{
	//	indices[i] = i;
	//}

	//// Release the index array since it is no longer needed.
	//delete[] indices;
	//indices = 0;

	// Set up the description of the dynamic particle buffer.
	D3D11_BUFFER_DESC particleBufferDesc;
	ZeroMemory(&particleBufferDesc, sizeof(D3D11_BUFFER_DESC));
	particleBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	particleBufferDesc.ByteWidth = sizeof(Particle) * maxParticles;
	particleBufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	particleBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | 0;
	particleBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	particleBufferDesc.StructureByteStride = sizeof(Particle);

	// Give the subresource structure a pointer to the vertex data.
	D3D11_SUBRESOURCE_DATA vertexData;
	ZeroMemory(&vertexData, sizeof(D3D11_SUBRESOURCE_DATA));
	vertexData.pSysMem = particles;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now finally create the particle buffer.
	hr = device->CreateBuffer(&particleBufferDesc, &vertexData, &particleBuffer);
	if (FAILED(hr))
		return false;

	// Setup an SRV for the particles
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN; // // Need to be UNKNOWN in case of structured buffer
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = maxParticles;

	hr = device->CreateShaderResourceView(particleBuffer, &srvDesc, &particleSrv);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Coldn't create SRV for particles!", L"PARTICLE ERROR", MB_OK);
		return false;
	}

	// Setup a UAV desc for the compute shader side of the particle managing.
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.Format = DXGI_FORMAT_UNKNOWN; // Need to be UNKNOWN in case of structured buffer
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = maxParticles;
	uavDesc.Buffer.Flags = 0;

	hr = device->CreateUnorderedAccessView(particleBuffer, &uavDesc, &particleUav);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Couldn't create UAV for particles!", L"PARTICLES ERROR", MB_OK);
		return false;
	}

	return true;
}

//bool Particles::UpdateBuffers(ID3D11DeviceContext* context)
//{
//
//	// DO THIS IN SHADER HANDLER INSTEAD
//	
//	
//	
//	// HRESULT hr;
//	// D3D11_MAPPED_SUBRESOURCE mappedResource;
//	// ID3D11Resource* particleResource;
//
//	// particleUav->GetResource(&particleResource);
//
//	// context->CopyResource(vertexBuffer, particleResource);
//	/*
//		Resource is mapped for writing, the previous contents of the resource will be undefined.
//		Map/Unmap is a common use with temporary buffers. - Dynamic usage of our vertexbuffer. (It is at least prefered)
//	*/
//
//	// Lock the vertex buffer
//	//hr = context->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);	// maybe D3D11_MAP_WRITE_NO_OVERWRITE
//	//if (FAILED(hr))
//	//{
//	//	return false;
//	//}
//
//	//// Get a pointer to the data in the vertex buffer
//	//vertexPtr = (Vertex*)mappedResource.pData;
//
//	//// Copy the data into the vertex buffer
//	//memcpy(vertexPtr, (void*)vertices, (sizeof(Vertex) * vertexCount));
//
//	//// Unlock the vertex buffer
//	//context->Unmap(vertexBuffer, 0);
//	return true;
//}

bool Particles::LoadParticles()
{
	Particle* particles = new Particle[maxParticles];
	float rndX, rndY, rndZ;

	for (int i = 0; i < maxParticles; ++i) 
	{
		rndX = 0;
		rndY = 50.0f;
		rndZ = 0.0f;
		particles[i].currentPosition = XMFLOAT3(rndX, rndY, rndZ);
		particles[i].oldPosition = XMFLOAT3(rndX, rndY, rndZ);
	}

	delete[] particles;

	return true;
}

//void Particles::CreateParticle(float frameTime)
//{
//	// Increment the frame time.
//	accumulatedTime += frameTime;
//
//	// Set emit particle to false for now.
//	bool emitParticle = false;
//
//	// Check if it is time to emit a new particle or not.
//	if (accumulatedTime > 0.1f) 
//	{
//		accumulatedTime = 0.0f;
//		emitParticle = true;
//	}
//}

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





