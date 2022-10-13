struct Particles
{
    float3 position;
    float3 velocity;
    float3 time;
};

cbuffer ParticleCount
{
    float numParticles;
    float3 padding;
};

// AppendStructuredBuffer<Particles> NewSimState : register(u0);
// ConsumeStructuredBuffer<Particles> CurrentSimState : register(u1);
RWStructuredBuffer<Particles> SimState : register(u0);

//cbuffer SimulationParameters
//{
//    float4 timeFactors;
//    float4 emitterLocation;
//    float4 consumerLocation;
//};

[numthreads(256, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    // Check for if this thread should run or not
    uint checkID = DTid.x + DTid.y * 256 + DTid.z * 256 * 256;
    
    // As long as the threads does not exceed the amount of particles...
    if (checkID < numParticles)
    {
        // Perform update process here...
        // Get current particle
        Particles particle = SimState[checkID];
        
        // Set the particles velocity for updating the position
        particle.velocity = (0.0f, -0.2f, 0.0f);
        
         // Update position of particle for making them fall down
        particle.position.xyz = particle.position.xyz + particle.velocity;
        
        SimState[checkID] = particle;
    }

}