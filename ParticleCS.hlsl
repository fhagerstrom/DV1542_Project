cbuffer Particles
{
    float deltaTime;
    float random;
    float2 padding;
};

RWBuffer<float> particlePosition;

[numthreads(8, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    // Keep track of particles current position.
    float3 currentPosition = float3(particlePosition[DTid.x * 3 + 0], particlePosition[DTid.y * 3 + 1], particlePosition[DTid.z * 3 + 2]);
    
    // Set position to fall down on y axis
    currentPosition.y = -2.5 * deltaTime;
    
    // If the current position on the y axis is less than 10...
    if(currentPosition.y < 10)
    {
        // Randomize the particles
        currentPosition = (10 * random);
    }
    
    // Update particle position
    particlePosition[DTid.x * 3 + 0] = currentPosition.x; // x
    particlePosition[DTid.x * 3 + 1] = currentPosition.y; // y
    particlePosition[DTid.x * 3 + 2] = currentPosition.z; // z
    
}