#pragma once

#include <random>
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

class ParticleSystem
{
public:
	ParticleSystem();
	~ParticleSystem();

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* context, LPCWSTR textureFile, Particle* particles, int maxParticles, int radius);
	void Render(ID3D11DeviceContext* context);
	void Shutdown();

	ID3D11ShaderResourceView* GetSrv() { return this->particleSrv; }
	ID3D11UnorderedAccessView* GetUav() { return this->particleUav; }
	ID3D11RenderTargetView* GetParticleRtv() { return this->particleRtv; }
	ID3D11Buffer* GetParticleBuffer() { return this->particleBuffer; }
	int GetMaxParticles() { return this->maxParticles; }
	int GetRadius() { return this->radius; }
	float GetQuadLength() { return this->quadLength; }

private:
	int maxParticles;
	int radius;
	float quadLength;

	Camera* camera;
	Texture* texture;

	ID3D11Buffer* particleBuffer;
	ID3D11RenderTargetView* particleRtv;
	ID3D11ShaderResourceView* particleSrv;
	ID3D11UnorderedAccessView* particleUav;

	bool InitializeBuffers(ID3D11Device* device, ID3D11DeviceContext* context, Particle* particles, int maxParticles);

	bool LoadParticles(int maxParticles, int radius);
	bool LoadTexture(ID3D11Device* device, LPCWSTR textureFile);
};