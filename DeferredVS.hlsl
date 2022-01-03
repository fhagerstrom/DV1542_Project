// Geometry Pass Vertex Shader

cbuffer constantBuffer : register(b1)
{
    float4x4 WVP;			//projectionMatrix
    float4x4 worldSpace;    // worldMatrix
}

struct VertexInput
{
    // The vertices of the model has a position, texcoord (UV), and a normal
    // WE're also going to calc a tangent for normal mapping

    float3 position : POSITION;
    float2 tex : TEXCOORD;
    float3 normal : NORMAL;
    float4 tangent : TANGENT;
};

struct VertexOutput
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
    float3 normal : NORMAL;
    float3 worldPos : WORLDPOS;
    float3 worldTangent : TANGENTWS;
    float3 worldBitangent : BITANGENTWS;
};

VertexOutput main(VertexInput input)
{
    VertexOutput output;

    // Convert to world space
    output.position = float4(input.position, 1);
    output.worldPos = mul(output.position, worldSpace);
    output.position = mul(output.position, WVP);
    output.tex = input.tex;
    output.normal = mul(float4(input.normal, 0), worldSpace);
    output.normal = normalize(output.normal);
    
    // NORMAL MAPPING
    // Construct tangent frame
    float3 tangentWS = normalize(mul(input.tangent, worldSpace));
    float3 bitangentWS = normalize(cross(output.normal, tangentWS)) * input.tangent.w;
    
    output.worldTangent = tangentWS;
    output.worldBitangent = bitangentWS;

    
    return output;
}
