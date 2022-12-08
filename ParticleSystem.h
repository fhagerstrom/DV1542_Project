#pragma once

#include <random>

#include "Camera.h"
#include "Texture.h"
#include "ParticleShaders.h"

class ParticleSystem
{
public:
	ParticleSystem(float quadLength, int maxParticles, int radius);
	~ParticleSystem();

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* context, LPCWSTR textureFile, Particle* particles, int maxParticles);
	void Render(ID3D11DeviceContext* context);
	void Shutdown();

	ID3D11ShaderResourceView* GetSrv() { return this->colorSRV; }
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
	ParticleShaders* particleShaders;

	ID3D11Buffer* particleBuffer;
	ID3D11RenderTargetView* particleRtv;
	ID3D11ShaderResourceView* colorSRV;
	ID3D11UnorderedAccessView* particleUav;
	ID3D11BlendState* blendState;

	bool InitializeBuffers(ID3D11Device* device, ID3D11DeviceContext* context, Particle* particles, int maxParticles);

	bool LoadParticles(int maxParticles, int radius);
	bool LoadTexture(ID3D11Device* device, LPCWSTR textureFile);
};