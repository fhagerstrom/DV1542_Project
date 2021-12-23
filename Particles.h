#pragma once
#include "Randomizer.h" 

class Particles
{
	friend class RenderParticles;
private:
	struct Particle
	{
		XMFLOAT3 pos;
		float velocity;
	};

	XMFLOAT3 bounds;
	XMFLOAT3 pos;

	float maxVelocity;
	float minVelocity;

	int particleCount;
	int maxParticles;

	float timeBetweenParticles;
	float timeSinceLastParticle;

	std::vector<Particle> particles; // vector containing each particle (used to loop through the vector and update the positions)
	std::vector<XMFLOAT3> eachPos; // vector containing each particles position ( used to update the vertexbuffer containgin each points position )

	ID3D11Buffer* vertexBuffer;

	void UpdateBuffer(ID3D11DeviceContext* context);

public:
	Particles() = default;
	Particles(ID3D11Device* device, int maxParticles, float maxVelocity, float minVelocity, XMFLOAT3 bounds, XMFLOAT3 position);
	~Particles();
	void Update(ID3D11DeviceContext* context, float dt);
	void Shutdown();

};