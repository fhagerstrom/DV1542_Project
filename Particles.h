#pragma once
#include "Texture.h"
#include "Vertex.h"

struct Particle
{
	XMFLOAT3 position;
	int seed;
	XMFLOAT3 velocity;
};

class Particles
{
private:
	bool InitializeBuffers(ID3D11Device* device);
	bool UpdateBuffers(ID3D11DeviceContext* context);

	void CreateParticle(float frameTime);
	void UpdateParticle(float frameTime);
	void DeleteParticles();
	bool LoadTexture(ID3D11Device* device, LPCWSTR textureFile);


	float deviationX, deviationY, deviationZ;
	float particleSpeed, particleSpeedVariation;
	float particleSize;
	int maxParticles;

	int currentParticleCount;
	float accumulatedTime;

	int vertexCount, indexCount;

	// Read about append / consume buffers 
	ID3D11Buffer* vertexBuffer, *indexBuffer;
	ID3D11Buffer* structuredBuffer;
	ID3D11ShaderResourceView* particleSrv;
	ID3D11UnorderedAccessView* particleUav;

	unsigned long* indices;
	Vertex* vertices;

	Particle* particles;
	Texture* texture;
	XMMATRIX world;

public:
	Particles();
	~Particles();
	
	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* context);
	bool Update(float frameTime, ID3D11DeviceContext* context);
	void Render(ID3D11DeviceContext* context);
	void Shutdown();

	void SetWorldMatrix(XMMATRIX world) { this->world = world; };
	XMMATRIX GetWorldMatrix() { return this->world; };

	ID3D11ShaderResourceView* GetTexture() { return this->particleSrv; }
	int GetIndexCount() { return this->indexCount; }
};