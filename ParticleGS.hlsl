cbuffer CBCamera
{
    float3 cameraPos;
    float padding;
};

struct GSInput
{
    float4 position : SV_POSITION;
};

struct GSOutput
{
	float4 position : SV_POSITION;
    float3 worldPos : POSITIONWS;
};

[maxvertexcount(3)]
void main(triangle GSOutput input[3], inout TriangleStream<GSOutput> output)
{
	// Define 2 out of 3 edges of the polygon, create a vector from vertex index 0 to the camera,
    // get the normal of the polygon by performing cross product of the edges.
    
    float3 U = input[2].worldPos - input[0].worldPos;
    float3 V = input[1].worldPos - input[0].worldPos;
    float W = cameraPos - input[0].worldPos;
    
    float3 normal = normalize(cross(V, U));
    
    // If the dot product of the normal and the negative value of the W vector is greater than 0...
    // We check the negative value of W because we take consideration into right-hand coordinates
    if (dot(normal, -W) >= 0)
    {
        // Return the vertices
        output.Append(input[2]);
        output.Append(input[1]);
        output.Append(input[0]);
    }
}