Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer CBLight : register(b0)
{
    float4 ambientLight;
    float4 diffuseLight;
    float4 specularLight;
    float3 lightPosition;
    float lightRange;
    
    float3 attenuation;
    float padding;
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
    int padding2;

    bool hasNormMap;
    float3 padding3;
}

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 worldPos : POSITIONWS;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 viewDirection : TEXCOORD1;
};

//struct PixelOutputType
//{
//    float4 color : SV_TARGET0;
//    float3 normal : SV_TARGET1;
//};

float4 main(PixelInputType input) : SV_TARGET
{
    float4 textureColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
    
    // Initialize colors that is going to be combined, light + material
    float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    // Sample the pixel color from the texture using the sampler at this texture coordinate location if it has a texture.
    if(hasTexture)
        textureColor = shaderTexture.Sample(SampleType, input.tex);
    
    // Normalize normals
    float3 normalizedNormal = normalize(input.normal);
    
    // Vector from lightsource to point thats going to be shaded, take distance from that vector
    float3 lightVector = lightPosition.xyz - input.worldPos.xyz;
    float distance = length(lightVector);
    
    // Normalize lightvector
    lightVector /= distance;
    
     // Calc ambient
    ambient = ambientLight * ambientMtl;
    
    if (distance > lightRange)
        return 0;
    
    // Diffuse factor
    float diffuseFactor = saturate(dot(lightVector, normalizedNormal));
    
    if (diffuseFactor > 0.0f)
    {
        // Specular shade with reflection
        float3 lightReflection = reflect(-lightVector, normalizedNormal);

        float specularShade = max(dot(lightReflection, input.viewDirection), 0.0f);
        float specularFactor = pow(specularShade, specularMtl.w); // w channel is the specular power

		// Set diffuse and specular
        diffuse = diffuseFactor * diffuseMtl * diffuseLight;
        specular = specularFactor * specularMtl * specularLight;
    }
    
    // Calc attenuation factor
    float attenuationFactor = 1.0f / attenuation[0] + attenuation[1] * distance + attenuation[2] * distance * distance;

	// Multiply attenuation to diffuse and specular
    diffuse *= attenuationFactor;
    specular *= attenuationFactor;
    
    diffuse = saturate(diffuse);
    specular = saturate(specular);
    ambient = saturate(ambient);
    
    float4 finalColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    finalColor = textureColor * (ambient + diffuse) + specular;

    return finalColor;
}