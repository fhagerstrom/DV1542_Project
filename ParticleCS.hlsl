struct Particles
{
    float3 position;
    float3 velocity;
    // float frameTime;
};

//cbuffer ParticleCount
//{
//    float numParticles;
//    float3 padding;
//};

// AppendStructuredBuffer<Particles> NewSimState : register(u0);
// ConsumeStructuredBuffer<Particles> CurrentSimState : register(u1);
RWStructuredBuffer<Particles> SimState : register(u0);

[numthreads(8, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    // Check for if this thread should run or not
    uint checkID = DTid.x + DTid.y * 8 + DTid.z * 8 * 8;
    
    // Get current particle
    Particles particle = SimState[checkID];
        
    // Keep track of the time for emitted particles
    // particle.frameTime += frac(dot(DTid.x + 1, float2(12.9898, 78.233)) * 43758.5453123);
        
    // Set the particles velocity for updating the position
    particle.velocity = (0.0f, -0.2f, 0.0f);
        
    // Update position of particle for making them fall down
    particle.position.xyz = particle.position.xyz + particle.velocity;
        
    SimState[checkID] = particle;

}