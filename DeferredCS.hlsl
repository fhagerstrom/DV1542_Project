RWTexture2D<float4> backBuffer;

Texture2D texPosition : register(t0);
Texture2D texNormal : register(t1);
Texture2D texAmbient : register(t2);
Texture2D texDiffuse : register(t3);
Texture2D texSpecular : register(t4);

cbuffer DeferredLighting
{
    float3 lightPosition;
    float lightRange;
}

// Eventually add more render targets when adding more techniques (e.g shadows)

[numthreads(32, 8, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    // Set values to its corresponding target
    float4 color = texDiffuse.Load(int3(DTid.xy, 0));
    float4 pos = texPosition.Load(int3(DTid.xy, 0));
    float4 normal = texNormal.Load(int3(DTid.xy, 0));
    float4 ambient = texAmbient.Load(int3(DTid.xy, 0));
    float4 specular = texSpecular.Load(int3(DTid.xy, 0));
    
    const int windowWidth = 1280;
    const int windowHeight = 720;
    
    if (length(normal.xyz) > 0.0f)
    {
        float4 lighting = (0.0f, 0.0f, 0.0f, 1.0f);
        float3 lightColor = (1.0f, 1.0f, 1.0f);
        
        // Add for loop after adding multiple lights, loop through all lights
        float3 lightVector = normalize(lightPosition.xyz - pos.xyz);
        float3 ambientLighting = ambient.xyz * lightColor;

        // TODO: Fix constant buffer(s) specifically for deferred HLSL
    }
    
}