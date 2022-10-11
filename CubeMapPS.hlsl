TextureCube cubeMap : register(t0);
SamplerState cubeSampler : register(s0);

cbuffer CBMaterial : register(b0)
{
    float4 diffuseMtl;
    float4 ambientMtl;
    float4 specularMtl;
    float4 reflectionMtl;

    float shine;
    bool hasTexture;
    bool hasReflection;
    int padding2;
}

cbuffer CBCamera : register(b1)
{
    float4 cameraPosition;
}

struct PixelInput
{
    float4 wvpPosition : SV_POSITION;
    float3 lightPosition : POSITION;
};

float4 main(PixelInput input) : SV_TARGET
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}