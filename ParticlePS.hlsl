Texture2D particleTexture : register(t0);
SamplerState particleSampler : register(s0);

struct PixelInput
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float4 main(PixelInput input) : SV_TARGET
{
    float4 output = particleTexture.Sample(particleSampler, input.tex);

    output.x = output.x - 0.4 + cos(0.1 * 0.4);
    output.y = output.y - 0.6 + cos(0.6 * 0.6);
    output.z = output.z - 0.4 + sin(0.3 * 0.4);


    return output;
}