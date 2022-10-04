RWTexture2D<float4> backBuffer;

Texture2D texPosition : register(t0);
Texture2D texNormal : register(t1);
Texture2D texAmbient : register(t2);
Texture2D texDiffuse : register(t3);
Texture2D texSpecular : register(t4);

static const int MAX_LIGHTS = 8;

cbuffer DeferredLighting : register(b0)
{
    float4 lightColor;
    float3 lightPosition;
    float3 cameraPosition;
    float padding;
    // int nrOfLights;
}

// Eventually add more render targets when adding more techniques (e.g shadows)

[numthreads(32, 8, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    // Set values to its corresponding target
    float4 pos = texPosition.Load(int3(DTid.xy, 0));
    float4 normal = texNormal.Load(int3(DTid.xy, 0));
    float4 ambient = texAmbient.Load(int3(DTid.xy, 0));
    float4 diffuse = texDiffuse.Load(int3(DTid.xy, 0));
    float4 specular = texSpecular.Load(int3(DTid.xy, 0));
    
    float4 lighting = (0.0f, 0.0f, 0.0f, 1.0f);
    float4 lightColor = (1.0f, 1.0f, 1.0f, 1.0f);
    float4 ambientLighting;
    float4 diffuseLighting;
    float4 specularLighting;
    
    const int windowWidth = 1280;
    const int windowHeight = 720;
    
    if (length(normal.xyz) > 0.0f)
    {
        // Add for loop after adding multiple lights, loop through all lights
        //for (int i = 0; i < nrOfLights; i++)
        // {
            float3 lightVector = normalize(lightPosition.xyz - pos.xyz);
            float viewDirection = normalize(cameraPosition.xyz - pos.xyz);
            
            // Calculate lighting
            // AMBIENT
            ambientLighting = ambient * lightColor;
            
            // DIFFUSE AND SPECULAR
            // Diffuse factor
            float diffuseFactor = saturate(dot(lightVector, normal.xyz));
    
            if (diffuseFactor > 0.0f)
            {
                // SPECULAR
                // Specular shade with reflection
                float3 lightReflection = reflect(-lightVector, normal.xyz);
                float specularShade = pow(max(dot(lightReflection, viewDirection), 0.0f), specular.w); // w = "shine"
                
                diffuseLighting = diffuseFactor * diffuse * lightColor;
                specularLighting = specular * specularShade;
            }
            
            lighting += saturate(ambientLighting + diffuseLighting) + specularLighting;
        // } // For loop clause
    }
    backBuffer[DTid.xy] = lighting;
}