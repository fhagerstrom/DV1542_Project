Texture2D shaderTexture : register(t0);

SamplerState sampleTypeWrap : register(s0);

struct PixelInput
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct PixelOutput
{
	float4 color : SV_Target0;
	float4 normal : SV_Target1;
};

PixelOutput main(PixelInput input) : SV_TARGET
{
    PixelOutput output;

    output.color = shaderTexture.Sample(sampleTypeWrap, input.tex);

    output.normal = float4(input.normal, 1.0f);

    return output;
}