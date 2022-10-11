Texture2D particleTexture : register(t0);
SamplerState particleSampler : register(s0);

struct PixelInput
{
    float4 position : SV_POSITION;
    float4 worldPosition : POSITION;
    float2 uv : TEXCOORD;
    float4 color : COLOR;
};

float4 main(PixelInput input) : SV_TARGET
{
    float4 textureColor = { 0.0f, 0.0f, 0.0f, 1.0f };
    float4 finalColor = { 0.0f, 0.0f, 0.0f, 1.0f };
    
    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColor = particleTexture.Sample(particleSampler, input.uv);
    
    // Combine the texture color and the particle color to get the final color result.
    finalColor = textureColor * input.color;
    
    return finalColor;
}