#pragma once
#include <d3d11.h>
#include <vector>
#include "Vertex.h"

struct Particle
{
	float posX, posY, posZ;
	float red, green, blue;
	float velocity;
	bool active;
};

class ParticleSystem
{
private:
	ID3D11Buffer* vBuffer;
	ID3D11Buffer* iBuffer;

	// TEXTURE

	float particleDeviationX;
	float particleDeviationY;
	float particleDeviationZ;
	float particleVelocity, particleVelocityVariation;
	float particleSize, particlesPerSecond;

	int maxParticles;
	int currentParticleCount;
	int accumulatedTime;
	int numVertices, numIndices;

	Particle* particleList;
	Vertex* vertices;

	bool InitParticles();
	bool InitBuffers();

	void EmitParticles(float);
	void UpdateParticles(float frameTime);
	void ExecuteOrder66Particles();

	// Do Render and update buffers in engine instead.

public:
	ParticleSystem();
	ParticleSystem(const ParticleSystem& other);
	~ParticleSystem();


	int GetIndexCount() { return numIndices; };


};