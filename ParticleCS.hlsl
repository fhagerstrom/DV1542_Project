struct Particles
{
    float3 position;
    float3 seed;
    float3 speed;
};

RWStructuredBuffer<Particles> SimState : register(u0);

[numthreads(128, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    // Check for if this thread should run or not
    uint checkID = DTid.x + DTid.y * 128 + DTid.z * 128 * 128;
    
    // Get current particle
    Particles particle = SimState[checkID];
    
    // Update position of particle for making them fall down
    particle.position.xyz = particle.position.xyz + float3(0.0f, -0.2f, 0.0f).xyz;
    
    // If a particle is out of bounds, move it to a random position near the camera
    
    
    
}