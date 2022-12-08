cbuffer CBView : register(b0)
{
    matrix view;
    float3 cameraPosition;
    // float time;
};

cbuffer CBConstantResource : register(b1)
{
    float quadLength;
    matrix projection;
}

struct GSInput // VSOutput
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
    // float3 color : COLOR;
};

struct GSOutput // PSInput
{
	float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
};

[maxvertexcount(4)]
void main(point GSInput input[1], inout TriangleStream<GSOutput> output)
{
    GSOutput p1 = (GSOutput)0;
    
    float4 normal = float4(input[0].position.xyz - cameraPosition, 1.0f);
    normal = mul(normal, view);

    float3 rightAxis = cross(float3(0.0f, 1.0f, 0.0f), normal.xyz);
    float3 upAxis = cross(normal.xyz, rightAxis);
    rightAxis = normalize(rightAxis);
    upAxis = normalize(upAxis);

    float4 rightVector = float4(rightAxis.xyz, 1.0f);
    float4 upVector = float4(upAxis.xyz, 1.0f);
    input[0].position = mul(input[0].position, view);

    // BOTTOM RIGHT
    p1.position = input[0].position + rightVector * (quadLength) + upVector * (quadLength);
    p1.tex = float2(1.0f, 0.0f);
    p1.position = mul(p1.position, projection);
    output.Append(p1);

    // TOP RIGHT
    p1.position = input[0].position + rightVector * (quadLength) + upVector * (-quadLength);
    p1.tex = float2(1.0f, 1.0f);
    p1.position = mul(p1.position, projection);
    output.Append(p1);

    // BOTTOM LEFT (ORIGO)
    p1.position = input[0].position + rightVector * (-quadLength) + upVector * (quadLength);
    p1.tex = float2(0.0f, 0.0f);
    p1.position = mul(p1.position, projection);
    output.Append(p1);

    // TOP LEFT
    p1.position = input[0].position + rightVector * (-quadLength) + upVector * (-quadLength);
    p1.tex = float2(0.0f, 1.0f);
    p1.position = mul(p1.position, projection);
    output.Append(p1);
    
    
    /* OLD CODE, KEEP AS BACKUP */
	//// Define 2 out of 3 edges of the polygon, create a vector from vertex index 0 to the camera,
 //   // get the normal of the polygon by performing cross product of the edges.
    
 //   float3 U = input[2].worldposition - input[0].worldposition;
 //   float3 V = input[1].worldposition - input[0].worldposition;
 //   float W = cameraposition - input[0].worldposition;
    
 //   float3 normal = normalize(cross(V, U));
    
 //   // If the dot product of the normal and the negative value of the W vector is greater than 0...
 //   // We check the negative value of W because we take consideration into right-hand coordinates
 //   if (dot(normal, -W) >= 0)
 //   {
 //       // Return the vertices
 //       output.Append(input[2]);
 //       output.Append(input[1]);
 //       output.Append(input[0]);
 //   }
}