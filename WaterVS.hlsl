cbuffer externalData : register(b0)
{
    matrix world;
    matrix view;
    matrix projection;
    matrix reflection;
}

struct VertexShaderInput
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

struct VertexToPixel
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float4 worldPos : POSITION;
    float2 uv : TEXCOORD;
    float3 reflWorldPos : POSITION1;
};

VertexToPixel main(VertexShaderInput input)
{
	// Set up output
    VertexToPixel output;

	// Calculate output position
    matrix worldViewProj = mul(mul(world, view), projection);
    output.position = mul(float4(input.position, 1.0f), worldViewProj);

	// Get the normal to the pixel shader
    output.normal = mul(input.normal, (float3x3) world); // ASSUMING UNIFORM SCALE HERE!!!  If not, use inverse transpose of world matrix
    output.tangent = mul(input.tangent, (float3x3) world); // Needed for normal mapping

	// Get world position of vertex
    output.worldPos = mul(float4(input.position, 1.0f), world);

    matrix worldReflProj = mul(mul(world, reflection), projection);
    output.reflWorldPos = mul(float4(input.position, 1.0f), worldReflProj);

	// Pass through the uv
    output.uv = input.uv;
    
    return output;
}