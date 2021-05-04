#include "Particles.h"

ParticleSystem::ParticleSystem()
{
	vBuffer = 0;
	iBuffer = 0;
}

ParticleSystem::ParticleSystem(const ParticleSystem& other)
{

}

ParticleSystem::~ParticleSystem()
{

}

bool ParticleSystem::InitParticles()
{
	// Set particle deviation where they can be located when emitted.
	particleDeviationX = 0.5f;
	particleDeviationY = 0.1f;
	particleDeviationZ = 2.0f;

	// Set particle speeds and it's variation.
	particleVelocity = 1.0f;
	particleVelocityVariation = 0.3f;

	particleSize = 0.1f;

	particlesPerSecond = 200.0f;

	maxParticles = 2500;

	particleList = new Particle[maxParticles];
	
	for (int i = 0; i < maxParticles; i++)
	{
		particleList[i].active = false;
	}
	
	currentParticleCount = 0;
	accumulatedTime = 0.0f;

	return true;
}

bool ParticleSystem::InitBuffers()
{

	return false;
}

void ParticleSystem::EmitParticles(float frameTime)
{
	bool emitParticle, found;
	float positionX, positionY, positionZ, velocity, red, green, blue;

	int index;

	// Increment frame time.
	accumulatedTime += frameTime;

	emitParticle = false;

	// Do a check for when to emit a new particle.
	if (accumulatedTime > (1000.0f / particlesPerSecond))
	{
		accumulatedTime = 0.0f;
		emitParticle = true;
	}

	// Emit one particle per frame if there is a particle.
	if ((emitParticle == true) && (currentParticleCount < (maxParticles - 1)))
	{
		currentParticleCount++;

		// Now generate the randomized particle properties.
		positionX = (((float)rand() - (float)rand()) / RAND_MAX) * particleDeviationX;
		positionY = (((float)rand() - (float)rand()) / RAND_MAX) * particleDeviationY;
		positionZ = (((float)rand() - (float)rand()) / RAND_MAX) * particleDeviationZ;

		velocity = particleVelocity + (((float)rand() - (float)rand()) / RAND_MAX) * particleVelocityVariation;

		red = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;
		green = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;
		blue = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;

		// Now since the particles need to be rendered from back to front for blending we have to sort the particle array.
		// We will sort using Z depth so we need to find where in the list the particle should be inserted.
		index = 0;

		found = false;
		while (!found)
		{
			if ((particleList[index].active == false) || (particleList[index].posZ < positionZ))
			{
				found = true;
			}
			else
			{
				index++;
			}
		}

		// Now that we know the location to insert into we need to copy the array over by one position from the index to make room for the new particle.
		int i = currentParticleCount;
		int j = i - 1;

		while (i != index)
		{
			particleList[i].posX = particleList[j].posX;
			particleList[i].posY = particleList[j].posY;
			particleList[i].posZ = particleList[j].posZ;
			particleList[i].red = particleList[j].red;
			particleList[i].green = particleList[j].green;
			particleList[i].blue = particleList[j].blue;
			particleList[i].velocity = particleList[j].velocity;
			particleList[i].active = particleList[j].active;
			i--;
			j--;
		}

		// Now insert it into the particle array in the correct depth order.
		particleList[index].posX = positionX;
		particleList[index].posY = positionY;
		particleList[index].posZ = positionZ;
		particleList[index].red = red;
		particleList[index].green = green;
		particleList[index].blue = blue;
		particleList[index].velocity = velocity;
		particleList[index].active = true;
	}
}

void ParticleSystem::UpdateParticles(float frameTime)
{
	for(int i = 0; i < currentParticleCount; i++)
	{
		particleList[i].posY = particleList[i].posY - (particleList[i].velocity * frameTime * 0.001f);
	}
}


void ParticleSystem::ExecuteOrder66Particles()
{
	// Execute all particles below a certain height range
	for(int i = 0; i < maxParticles; i++)
	{
		if((particleList[i].active == true) && (particleList[i].posY < -3.0f))
		{
			particleList[i].active = false;
			currentParticleCount--;
			// Shift particles in array to keep them sorted correctly and remove destroyed particles
			for(int j = 0; j < maxParticles; j++)
			{
				particleList[j].posX = particleList[j+1].posX;
				particleList[j].posY = particleList[j+1].posY;
				particleList[j].posZ = particleList[j+1].posZ;
				particleList[j].red = particleList[j+1].red;
				particleList[j].green = particleList[j+1].green;
				particleList[j].blue = particleList[j+1].blue;
				particleList[j].velocity = particleList[j+1].velocity;
				particleList[j].active = particleList[j+1].active;
			}
		}
	}
}


