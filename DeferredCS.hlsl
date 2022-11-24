RWTexture2D<float4> backBuffer;

Texture2D texPosition : register(t0);
Texture2D texNormal : register(t1);
Texture2D texAmbient : register(t2);
Texture2D texDiffuse : register(t3);
Texture2D texSpecular : register(t4);
Texture2DArray shadowMap : register(t5);

static const int MAX_LIGHTS = 4;

cbuffer DeferredLighting : register(b0)
{
    float4 ambientLight;
    float4 diffuseLight;
    float4 specularLight;
    float3 lightPosition;
    float lightRange;
    
    float3 attenuation;
    float padding;
    
    float3 cameraPosition;
    float padding2;
    // int nrOfLights;
    
    matrix projection;
    matrix lightVP;
}

// Eventually add more render targets when adding more techniques (e.g shadows)

[numthreads(32, 8, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
     // Set shadowmapping size to same as window size
    const int shadowWidth = 1280;
    const int shadowHeight = 720;
    
    // Set values to its corresponding target
    float4 pos = texPosition[DTid.xy];
    float4 normal = texNormal[DTid.xy];
    float4 ambient = texAmbient[DTid.xy];
    float4 diffuse = texDiffuse[DTid.xy];
    float4 specular = texSpecular[DTid.xy];
    
    float4 lighting = (0.0f, 0.0f, 0.0f, 0.0f);
    // float4 lightColor = (1.0f, 1.0f, 1.0f, 1.0f);
    
    float4 ambientLighting = (0.0f, 0.0f, 0.0f, 0.0f);
    float4 diffuseLighting = (0.0f, 0.0f, 0.0f, 0.0f);
    float4 specularLighting = (0.0f, 0.0f, 0.0f, 0.0f);
    
    // Add for loop after adding multiple lights, loop through all lights
    //for (int i = 0; i < nrOfLights; i++)
    // {
    float3 lightVector = normalize(lightPosition.xyz - pos.xyz);
    float viewDirection = normalize(cameraPosition.xyz - pos.xyz);
    float distance = length(lightVector);
    
    // Normalize lightvector
    lightVector /= distance;
    
    // SHADOWMAPPING
    float4 shadowCam = pos;
    float4 shadowVector = mul(shadowCam, lightVP);
    float4 shadowCoords = shadowVector * float4(0.5, -0.5, 1.0f, 1.0f) + (float4(0.5f, 0.5f, 0.0f, 0.0f) * shadowVector.w);
    
    shadowCoords.xyz = shadowCoords.xyz / shadowCoords.w;
    
    float4 SM = shadowMap.Load((shadowCoords.x * shadowWidth, shadowCoords.y * shadowHeight), 0);
    
    // Calculate lighting
    // AMBIENT
    ambientLighting = ambient * ambientLight /** lightColor*/;
    
    //if (distance > lightRange)
    //    backBuffer[DTid.xy] = 0;
    
    // Set values for shadow mapping and necessary checks
    float bias = 0.00001f;
    
    // If we are outside the shadow...
    if (SM.r > shadowCoords.z - bias &&
                shadowCoords.z <= 1.0f && 
				shadowCoords.x < 1 && shadowCoords.x > 0 &&
				shadowCoords.y < 1 && shadowCoords.y > 0 &&
                dot(normal.xyz, lightVector.xyz) > -0.1
        )
    {
        // DIFFUSE CALCULATION
        float diffuseAmount = max(dot(normal.xyz, lightVector), 0);
        diffuseLighting = diffuseAmount * diffuse * diffuseLight /** lightColor*/;
        
        // SPECULAR CALCULATION
        float3 lightReflection = reflect(-lightVector, normal.xyz);
        float specularShade = pow(max(dot(viewDirection, lightReflection), 0), specular.w);
        specularLighting = specular * specularShade * specularLight /** lightColor*/;
        
        ambientLighting = saturate(ambientLighting);
        diffuseLighting = saturate(diffuseLighting);
        specularLighting = saturate(specularLighting);
        
        lighting = (ambientLighting + diffuseLighting) + specularLighting;
    }
    
    else
    {
        // We are inside the shadow
        lighting = ambient * diffuse /** lightColor*/;
    }
            
    // DIFFUSE
    // Diffuse factor
    float diffuseFactor = saturate(dot(lightVector, normal.xyz));
    diffuseLighting = diffuseFactor * diffuse * diffuseLight /** lightColor*/;
        
    // SPECULAR
    // Specular shade with reflection
    float3 lightReflection = reflect(-lightVector, normal.xyz);
    float specularShade = pow(max(dot(lightReflection, viewDirection), 0.0f), specular.w); // w = shininess
    specularLighting = specular * specularShade * specularLight /** lightColor*/;
    
    // Calc attenuation factor
    float attenuationFactor = 1.0f / attenuation[0] + attenuation[1] * distance + attenuation[2] * distance * distance;
    
    // Multiply attenuation to diffuse and specular
    diffuseLighting *= attenuationFactor;
    specularLighting *= attenuationFactor;
            
    diffuseLighting = saturate(diffuseLighting);
    specularLighting = saturate(specularLighting);
    ambientLighting = saturate(ambientLighting);
        
    lighting = (ambientLighting + diffuseLighting) + specularLighting;
    // } // For loop clause
    backBuffer[DTid.xy] = lighting;
}