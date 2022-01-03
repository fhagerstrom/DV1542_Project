// Geometry Pass Pixel Shader

Texture2D diffuseTex : register(t0);
Texture2D normalMap : register(t1);

SamplerState wrapSampler : register(s0);
SamplerState clampSampler : register(s1);

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
    float3 normal : NORMAL;
    float3 worldPos : WORLDPOS;
    float3 tangent : TANGENTWS;
    float3 bitangent : BITANGENTWS;
};

struct PixelOutput
{
    float4 position : SV_TARGET0;
    float3 normal : SV_TARGET1;
    float3 worldPos : SV_TARGET2;
    float4 diffuse : SV_TARGET3;
    float4 ambientMTL : SV_TARGET4;
    float4 diffuseMTL : SV_TARGET5;
    float4 specularMTL : SV_TARGET6;
    float4 lightClipPos : SV_TARGET7;
};

cbuffer mtlData : register(b0)
{
    float4 kA;
    float4 kD;
    float4 kS;
}

cbuffer LightMatrix : register(b1)
{
    float4x4 lightMatrix;
}

PixelOutput main(PixelInput input)
{
    PixelOutput output;

    output.position = input.position;
    output.worldPos = input.worldPos;
    output.diffuse = diffuseTex.Sample(wrapSampler, input.tex);
    output.ambientMTL = kA;
    output.diffuseMTL = kD;
    output.specularMTL = kS;
    
    // NORMAL MAPPING
    // Normalize tangent frame with a TBN matrix
    float3x3 tangentframeWS = float3x3(normalize(input.tangent),
                                    normalize(input.bitangent),
                                    normalize(input.normal));
    
    // Sample the tangent space normal map and decompress
    float3 normalTS = normalMap.Sample(wrapSampler, input.tex);
    normalTS = normalize(normalTS * 2.0f - 1.0f);
    
    // Convert to world space
    float3 normalWS = mul(normalTS, tangentframeWS);
    
    // Output values
    output.normal = float4(normalWS, 1.0f);
    output.lightClipPos = mul(float4(output.worldPos, 1.0f), lightMatrix);

    return output;
}

