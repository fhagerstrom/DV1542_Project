Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer CBLight : register(b0)
{
    float4 ambientColor;
    float4 diffuseColor;
    float4 specularColor;
    float3 lightDirection;
    float specularPower;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 viewDirection : TEXCOORD1;
};

float4 main(PixelInputType input) : SV_TARGET
{
    float4 textureColor;
    float3 lightDir;
    float lightIntensity;
    float4 color;
    float3 reflection;
    float4 specular;
    
    color = ambientColor;
    
    // Initialize the specular color.
    specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColor = shaderTexture.Sample(SampleType, input.tex);
    
     // Invert the light direction for calculations.
    lightDir = -lightDirection;

    // Calculate the amount of light on this pixel.
    lightIntensity = dot(lightDir, input.normal);
    
    if (lightIntensity > 0.0f)
    {
        // Determine the final diffuseColor color based on the diffuseColor color and the amount of light intensity.
        color += (diffuseColor * lightIntensity);
        
        // Saturate the ambientColor and diffuseColor color.
        color = saturate(color);
        
        // Calculate the reflection vector based on the light intensity, normal vector, and light direction.
        reflection = normalize(2 * lightIntensity * input.normal - lightDir);
        
        // Determine the amount of specular light based on the reflection vector, viewing direction, and specular power.
        specular = pow(saturate(dot(reflection, input.viewDirection)), specularPower);
    }
    
    // Multiply the texture pixel and the input color to get the textured result.
    color = color * textureColor;
    
    // Set specular component as the last calculation.
    color = saturate(color + specular);

    return color;
}