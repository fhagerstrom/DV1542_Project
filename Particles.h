#pragma once
#include "DxHandler.h"
#include "Texture.h"
#include "Vertex.h"

struct Particle
{
	XMFLOAT3 position;
	int seed;
	float speed;
};

class Particles
{
private:
	bool InitializeBuffers(ID3D11Device* device);
	void UpdateBuffers(ID3D11DeviceContext* context);

	void CreateParticle(float frameTime);
	void UpdateParticle(float frameTime);
	void DeleteParticles();

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

	ID3D11ShaderResourceView* GetTexture();
	int GetIndexCount();
};

