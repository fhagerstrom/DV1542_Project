Texture2D texAmbient : register(t0);
Texture2D texDiffuse : register(t1);
Texture2D texSpecular : register(t2);
SamplerState deferredSampler : register(s0);

cbuffer CBMaterial : register(b1)
{
    float4 diffuseMtl;
    float4 ambientMtl;
    float4 specularMtl;
    float4 reflectionMtl;

    float shine;
    bool hasTexture;
    bool hasReflection;
    int padding;
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float4 worldPos : POSITIONWS;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 viewDirection : TEXCOORD1;
};

struct PixelOutput
{
    float4 position : SV_Target0;
    float4 normal : SV_Target1;
    float4 ambient : SV_Target2;
    float4 diffuse : SV_Target3;
    float4 specular : SV_Target4;
};

PixelOutput main(PixelInput input)
{
    PixelOutput output;
    
    output.position = input.worldPos;
    output.normal = float4(input.normal.xyz, 1);
    
    if (hasTexture)
    {
        output.ambient = texAmbient.Sample(deferredSampler, input.tex) * ambientMtl;
        output.diffuse = texDiffuse.Sample(deferredSampler, input.tex) * diffuseMtl;
        output.specular = texSpecular.Sample(deferredSampler, input.tex) * specularMtl;
    }
    else
    {
        output.ambient = ambientMtl;
        output.diffuse = diffuseMtl;
        output.specular = specularMtl;
    }
    
    output.specular.w = specularMtl.w;
    
	return output;
}