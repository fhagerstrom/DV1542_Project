#include "Defines.h"
#include "Particles.h"


Particles::Particles(ID3D11Device* device, int maxParticles, float maxVelocity, float minVelocity, XMFLOAT3 bounds, XMFLOAT3 position)
    :maxParticles(maxParticles), maxVelocity(maxVelocity), minVelocity(minVelocity), bounds(bounds), pos(pos),
    particleCount(0), timeBetweenParticles(0.02f), timeSinceLastParticle(0.0f)
{
    // Create the vertexbuffer 

    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;					// Map UnMap
    bufferDesc.ByteWidth = sizeof(XMFLOAT3) * maxParticles; // Each particle has a postition (XMFLOAT3) * max amount of particles to set correct size of the buffer
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;

    eachPos.reserve(sizeof(XMFLOAT3) * maxParticles); // Reserve size of vector to 

    D3D11_SUBRESOURCE_DATA data = {};
    data.pSysMem = eachPos.data();
    data.SysMemPitch = 0;
    data.SysMemSlicePitch = 0;

    HRESULT hr = device->CreateBuffer(&bufferDesc, &data, &vertexBuffer);
    if FAILED(hr)
    {
        std::cout << "FAILED TO CREATE BUFFER" << std::endl;
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
        timeSinceLastParticle += dt;

        // Create 
        if (timeSinceLastParticle >= timeBetweenParticles)
        {
            timeSinceLastParticle = 0;

            Particle newParticle = {};

            // Randomize the start x & z position based on bounds and position (parameters for creating the system)
            newParticle.pos.x = Randomizer::Real(pos.x - bounds.x / 2.0f, pos.x + bounds.x / 2.0f);
            newParticle.pos.z = Randomizer::Real(pos.z - bounds.z / 2.0f, pos.z + bounds.z / 2.0f);
            // Set the starting y-position on the top of the bounds
            newParticle.pos.y = bounds.y;
            // Randomize the velocity of the individual particle
            newParticle.velocity = Randomizer::Real(minVelocity, maxVelocity);

            // Add to vector and increase the count
            particles.push_back(newParticle);
            particleCount++;
        }
    }
    // Update the particles positions
    for (auto& particle : particles)
    {
        particle.pos.y -= particle.velocity * dt;
        if (particle.pos.y <= pos.y - bounds.y)
        {
            particle.pos.y = pos.y;
        }
        eachPos.push_back(particle.pos);
    }
    // Update the buffer containing the positions
    UpdateBuffer(context);
}

// Function to update the buffer containing the positions of every particles point used in the geometryshader
void Particles::UpdateBuffer(ID3D11DeviceContext* context)
{
    D3D11_MAPPED_SUBRESOURCE mappedResource = {};

    HRESULT hr = context->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(hr))
    {
        std::cout << "FAILED TO TO CREATE PARTICLE VERTEXBUFFER" << std::endl;
    }
    memcpy(mappedResource.pData, eachPos.data(), sizeof(XMFLOAT3) * particleCount);
    context->Unmap(vertexBuffer, 0);
}


void Particles::Shutdown() 
{ 
    vertexBuffer->Release(); 
}