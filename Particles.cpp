#include "Particles.h"

Particles::Particles()
{
}

Particles::~Particles()
{
}

bool Particles::Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
{
	HRESULT hr;

	// Set deviation on particles. This is a point from where the particle will "spawn" and then be randomized from.
	deviationX = 30.0f;
	deviationY = 0.2f;
	deviationZ = 30.0f;

	particleSpeed = 15.0f;
	particleSpeedVariation = 0.5f;

	particleSize = 0.5f;

	maxParticles = 100;

	// Set accumulated time to 0. 
	// This is from the beginning and the time will increment based on delta time.
	accumulatedTime = 0.0f;

	return true;
}


