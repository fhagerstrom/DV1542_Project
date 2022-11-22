#pragma once
#include <d3d11.h>

#include "Camera.h"
#include "Texture.h"

using namespace DirectX;

struct Particle
{
	Particle() {};

	XMFLOAT3 currentPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 oldPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);

	XMFLOAT3 SetPosition(XMFLOAT3& position) { currentPosition = position; }
	XMFLOAT3 GetPosition() { return currentPosition; }
};

class Particles
{
public:
	Particles();
	~Particles();

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* context, int maxParticles);
	bool Update(float deltaTime);
	void Render(ID3D11DeviceContext* context, int maxParticles);
	void Shutdown();

	// ID3D11ShaderResourceView* GetTexture() { return this->particleSrv; }
	ID3D11UnorderedAccessView* GetUav() { return particleUav; }
	int GetMaxParticles() { return this->maxParticles; }
	ID3D11Buffer* GetParticleBuffer() { return this->particleBuffer; }

private:
	int maxParticles;
	float quadLength;

	Camera* camera;
	Texture* texture;
	Particle* particles;

	ID3D11Buffer* particleBuffer;
	ID3D11ShaderResourceView* particleSrv;
	ID3D11UnorderedAccessView* particleUav;

	bool InitializeBuffers(ID3D11Device* device, int maxParticles);
	//bool UpdateBuffers(ID3D11DeviceContext* context);

	bool LoadParticles();
	// void CreateParticle(float frameTime);
	bool LoadTexture(ID3D11Device* device, LPCWSTR textureFile);
};