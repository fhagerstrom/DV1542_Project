#pragma once
#include "Randomizer.h" 

class Particles
{
	friend class RenderParticles;
private:
	struct Particle
	{
		XMFLOAT3 position;
		float velocity;
	};

	XMFLOAT3 bounds;
	XMFLOAT3 position;
	float maxVelocity;
	float minVelocity;
	int particleCount;
	int maxParticles;
	float timeBetweenEachParticle;
	float timePassedSinceLastParticle;
	std::vector<Particle> particles; //each particle (looping through the vector and update the positions of particles)
	std::vector<XMFLOAT3> eachPos; //each particle's position ( to update vbuffer containing each point's position)
	ID3D11Buffer* vBuffer;

	void UpdateBuffer(ID3D11DeviceContext* context);

public:
	Particles() = default;
	Particles(ID3D11Device* device, int maxParticles, float maxVelocity, float minVelocity, XMFLOAT3 bounds, XMFLOAT3 position);
	~Particles();
	void Update(ID3D11DeviceContext* context, float dt);
	void Shutdown();

};