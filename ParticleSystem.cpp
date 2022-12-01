#include "ParticleSystem.h"

ParticleSystem::ParticleSystem()
{
	maxParticles = 50;
	radius = 100;
	quadLength = 50.0f;
}

ParticleSystem::~ParticleSystem()
{
}

bool ParticleSystem::Initialize(ID3D11Device* device, ID3D11DeviceContext* context, LPCWSTR textureFile, Particle* particles, int maxParticles, int radius)
{
	bool result;

	result = InitializeBuffers(device, context, particles, maxParticles);
	if (!result)
	{
		return false;
	}

	result = LoadParticles(maxParticles, radius);
	if (!result)
	{
		return false;
	}

	result = LoadTexture(device, textureFile);
	if (!result)
	{
		return false;
	}

	return true;
}

void ParticleSystem::Render(ID3D11DeviceContext* context)
{
}

void ParticleSystem::Shutdown()
{
}

bool ParticleSystem::InitializeBuffers(ID3D11Device* device, ID3D11DeviceContext* context, Particle* particles, int maxParticles)
{
	HRESULT hr;

	// Set up the description of the dynamic particle buffer.
	D3D11_BUFFER_DESC particleBufferDesc;
	ZeroMemory(&particleBufferDesc, sizeof(D3D11_BUFFER_DESC));
	particleBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	particleBufferDesc.ByteWidth = sizeof(Particle) * maxParticles;
	particleBufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	particleBufferDesc.CPUAccessFlags = 0;
	particleBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	particleBufferDesc.StructureByteStride = sizeof(Particle);

	// Give the subresource structure a pointer to the vertex data.
	D3D11_SUBRESOURCE_DATA bufferData;
	ZeroMemory(&bufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	bufferData.pSysMem = particles;
	bufferData.SysMemPitch = 0;
	bufferData.SysMemSlicePitch = 0;

	// Now finally create the particle buffer.
	hr = device->CreateBuffer(&particleBufferDesc, &bufferData, &particleBuffer);
	if (FAILED(hr))
		return false;

	// Setup an SRV for the particles
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN; // Need to be UNKNOWN in case of structured buffer
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

bool ParticleSystem::LoadParticles(int maxParticles, int radius)
{
	Particle* particles = new Particle[maxParticles];
	float rndX, rndY, rndZ;

	std::mt19937 mt; // mt = Mersenne Twister, generate random numbers
	std::uniform_real_distribution<float> dist(-radius, radius); // Produces random floating-point values between two numbers

	for (int i = 0; i < maxParticles; ++i) 
	{
		rndX = dist(mt);
		rndY = dist(mt);
		rndZ = dist(mt);

		rndX = 0;
		rndY = 15.0f;
		rndZ = 0.0f;
		particles[i].currentPosition = XMFLOAT3(rndX, rndY, rndZ);
		particles[i].oldPosition = XMFLOAT3(rndX, rndY, rndZ);
	}

	// delete[] particles;

	return true;
}

bool ParticleSystem::LoadTexture(ID3D11Device* device, LPCWSTR textureFile)
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





