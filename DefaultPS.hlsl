Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer CBLight : register(b0)
{
    float4 ambientLight;
    float4 diffuseLight;
    float4 specularLight;
    float3 lightDirection;
    float specularPower;
};

cbuffer CBMaterial : register(b1)
{
    float4 diffuseMtl;
    float4 ambientMtl;
    float4 specularMtl;
    float4 reflectionMtl;

    float shine;
    bool hasTexture;
    bool hasReflection;

    float4 translation;

    bool hasNormMap;
    float3 padding;
}

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 worldPos : POSITIONWS;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 viewDirection : TEXCOORD1;
};

float4 main(PixelInputType input) : SV_TARGET
{
    float4 textureColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
    
    // Initialize colors that is going to be combined, light + material
    float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    // Calc ambient
    ambient = ambientLight * ambientMtl;
    
    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColor = shaderTexture.Sample(SampleType, input.tex);
    
    // Normalize normals
    float3 normalizedNormal = normalize(input.normal);
    
    // Vector from lightsource to point thats going to be shaded, take distance from that vector
    float3 lightVector = -lightDirection.xyz /*- input.worldPos.xyz*/;
    float distance = length(lightVector);
    
    // Normalize lightvector
    lightVector /= distance;
    
    // Diffuse factor
    float diffuseFactor = saturate(dot(lightVector, normalizedNormal));
    
    if (diffuseFactor > 0.0f)
    {
        // Specular shade with reflection
        float3 lightReflection = reflect(-lightVector, normalizedNormal);

        float specularShade = max(dot(lightReflection, input.viewDirection), 0.0f);
        float specularFactor = pow(specularShade, specularPower);

		// Set diffuse and specular
        diffuse = diffuseFactor * diffuseLight; // MULTIPLY BY MATERIAL
        specular = specularFactor * specularLight; // MULTIPLY BY MATERIAL
    }
    
    diffuse = saturate(diffuse);
    specular = saturate(specular);
    ambient = saturate(ambient);
    
    float4 finalColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    finalColor = textureColor * (ambient + diffuse) + specular;

    return finalColor;
}