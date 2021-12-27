#include "Defines.h"
#include "Particles.h"


Particles::Particles(ID3D11Device* device, int maxParticles, float maxVelocity, float minVelocity, XMFLOAT3 bounds, XMFLOAT3 position)
    :maxParticles(maxParticles), maxVelocity(maxVelocity), minVelocity(minVelocity), bounds(bounds), position(position),
    particleCount(0), timeBetweenEachParticle(0.02f), timePassedSinceLastParticle(0.0f)
{
    // Create vertexbuffer 
    D3D11_BUFFER_DESC desc = {};
    desc.Usage = D3D11_USAGE_DYNAMIC;//map and unmap
    desc.ByteWidth = sizeof(XMFLOAT3) * maxParticles; //each particles needs it's position (an xmfloat3)*maxAmountOfParticles
    //this sets correct buffer size

    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;

    eachPos.reserve(sizeof(XMFLOAT3) * maxParticles); // Reserve size of vector to following:
    D3D11_SUBRESOURCE_DATA data = {};
    data.pSysMem = eachPos.data();
    data.SysMemPitch = 0;
    data.SysMemSlicePitch = 0;

    HRESULT hr = device->CreateBuffer(&desc, &data, &vBuffer);
    if FAILED(hr)
    {
        std::cerr << "ERROR! COULD NOT CREATE PARTICLE BUFFER!" << std::endl;
    }
}


Particles::~Particles()
{

}

void Particles::Update(ID3D11DeviceContext* context, float dt)
{
    eachPos.clear(); // Clear the data within the positions-vector

    // If we dont have max amount of particles - create a new one
    if (particleCount < maxParticles)
    {
        // Add the deltatime to make the particles spawn "smoothly"
        timePassedSinceLastParticle += dt;

        // Create 
        if (timePassedSinceLastParticle >= timeBetweenEachParticle)
        {
            timePassedSinceLastParticle = 0;
            Particle newParticle = {};
            // Randomize the positions
            newParticle.position.x = Randomizer::Real(position.x - bounds.x / 2.0f, position.x + bounds.x / 2.0f);
            newParticle.position.z = Randomizer::Real(position.z - bounds.z / 2.0f, position.z + bounds.z / 2.0f);
            // starting y position is at the top of the bounds
            newParticle.position.y = bounds.y;
            // Randomize the velocity for every particle
            newParticle.velocity = Randomizer::Real(minVelocity, maxVelocity);

            // Add each particle to vector
            particles.push_back(newParticle);
            particleCount++;
        }
    }
    // Update position of particles
    for (auto& particle : particles)
    {
        particle.position.y -= particle.velocity * dt;
        if (particle.position.y <= position.y - bounds.y)
        {
            particle.position.y = position.y;
        }
        eachPos.push_back(particle.position);
    }
    // Update the buffer with all positions
    UpdateBuffer(context);
}

void Particles::UpdateBuffer(ID3D11DeviceContext* context)
{
    D3D11_MAPPED_SUBRESOURCE mappedResource = {};
    HRESULT hr = context->Map(vBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(hr))
    {
        std::cerr << "ERROR! COULD NOT CREATE PARTICLE VERTEXBUFFER!" << std::endl;
    }
    memcpy(mappedResource.pData, eachPos.data(), sizeof(XMFLOAT3) * particleCount);
    context->Unmap(vBuffer, 0);
}

void Particles::Shutdown() 
{ 
    vBuffer->Release(); 
}