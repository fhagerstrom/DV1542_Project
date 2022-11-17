cbuffer CBConstantParticles : register(b0)
{
    uint maxParticles;
};

cbuffer CBDynamicParticles : register(b1)
{
    float deltaTime;
    float3 padding;
}

struct Particle
{
    float3 currentPosition;
    float3 oldPosition;
};

RWStructuredBuffer<Particle> Particles : register(u0);

[numthreads(32, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    Particle particleProcess = Particles[DTid.x];
    
    // Manipulate particleProcess to update particles.
    float3 tempPosition;
    
    // Make them fall down like rain.
    tempPosition = 2.0f * (particleProcess.currentPosition - particleProcess.oldPosition) * deltaTime;
    
    particleProcess.oldPosition = particleProcess.currentPosition;
    particleProcess.currentPosition = tempPosition;
    
    // Keep all the particles inside a specific range.
    if (length(particleProcess.currentPosition) > 55)
    {
        float3 norm = normalize(particleProcess.currentPosition);
        particleProcess.currentPosition = norm * 55;
    }
    
    Particles[DTid.x] = particleProcess;
    
    /* OLD CODE, KEEP JUST IN CASE */
    //// Keep track of particles current position.
    //currentPos = float3(particlePosition[DTid.x * 3 + 0], particlePosition[DTid.y * 3 + 1], particlePosition[DTid.z * 3 + 2]);
    
    //// Set position to fall down on y axis
    //currentPosition.y = -2.5 * deltaTime;
    
    //// If the current position on the y axis is less than 10...
    //if(currentPosition.y < 10)
    //{
    //    // Randomize the particles
    //    currentPosition = (10 * random);
    //}
}