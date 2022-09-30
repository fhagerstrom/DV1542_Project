cbuffer CBPerObject : register(b0)
{
    matrix wvp;
	matrix world;
	//matrix viewMatrix;
	//matrix projectionMatrix;
};

cbuffer cameraBuffer : register(b1)
{
    float3 cameraPos;
    float padding;
};

struct VertexInput
{
    float3 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float4 worldPos : POSITIONWS;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 viewDirection : TEXCOORD1;
};

PixelInput main(VertexInput input)
{
    PixelInput output;

	// Calculate the position of the vertex against the world View Projection (WVP) and world matrix
    output.position = mul(float4(input.position, 1.0f), wvp);
    output.worldPos = mul(float4(input.position, 1.0f), world);
    
    
	// Store the texture coordinates for the pixel shader.
    output.tex = input.tex;
    
    output.normal = mul(input.normal, (float3x3)world);
    
    // Same goes for normals.
    output.normal = normalize(output.normal);

    // Determine the viewing direction based on the position of the camera and the position of the vertex in the world.
    output.viewDirection = cameraPos.xyz - output.worldPos.xyz;
	
    // Normalize the viewing direction vector.
    output.viewDirection = normalize(output.viewDirection);
    
    return output;
}