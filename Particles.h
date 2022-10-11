#pragma once
#include "DxHandler.h"
#include "Texture.h"
#include "Vertex.h"


class Particles
{
private:
	bool LoadTexture(ID3D11Device* device, LPCWSTR textureName);
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
	ID3D11Buffer* vertexBuffer, *indexBuffer;
	Vertex* vertices;

	Texture* texture;
	XMMATRIX world;

public:
	Particles();
	~Particles();
	
	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* context);
	bool Update(float frameTime, ID3D11DeviceContext* context);
	void Render(ID3D11DeviceContext* context);
	void Shutdown();

	void SetWorldMatrix(XMMATRIX world);
	XMMATRIX GetWorldMatrix();

	ID3D11ShaderResourceView* GetTexture();
	int GetIndexCount();
};

