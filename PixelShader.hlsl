// External texture-related data
Texture2D Texture : register(t0);
Texture2D NormalMap : register(t1);
SamplerState Sampler : register(s0);

struct PointLight
{
	float4 color;
	float3 position;
};

struct SpotLight
{
	float3 direction;
	float spotPower;
};

cbuffer lightData : register(b0)
{
	float4 DirLightColor;
	float3 DirLightDirection;
	
	// Point light sources for spotlight
	PointLight pointLight1;
	PointLight pointLight2;

	PointLight pointLight3;
	PointLight pointLight4;

	SpotLight spotLight1;
	SpotLight spotLight2;	

	float3 CameraPosition;
};

struct VertexToPixel
{
	float4 position	: SV_POSITION;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 worldPos : POSITION;
	float2 uv : TEXCOORD;
};

float4 main(VertexToPixel input) : SV_TARGET
{
	input.normal = normalize(input.normal);
	input.tangent = normalize(input.tangent);

	// Read and unpack normal from map
	float3 normalFromMap = NormalMap.Sample(Sampler, input.uv).xyz * 2 - 1;

	// Transform from tangent to world space
	float3 n = input.normal;
	float3 t = normalize(input.tangent - n * dot(input.tangent, n));
	float3 b = cross(t, n);

	float3x3 tbn = float3x3(t, b, n);
	input.normal = normalize(mul(normalFromMap, tbn));

	// Sample the texture
	float4 textureColor = Texture.Sample(Sampler, input.uv);

	// Directional light calculation
	float dirLightAmount = saturate(dot(input.normal, -normalize(DirLightDirection)));

	// Point light calculation
	float3 dirToPointLight1 = normalize(pointLight1.position - input.worldPos);
	float pointLightAmount1 = saturate(dot(input.normal, dirToPointLight1));

	float3 dirToPointLight2 = normalize(pointLight2.position - input.worldPos);
	float pointLightAmount2 = saturate(dot(input.normal, dirToPointLight2));

	float3 dirToPointLight3 = normalize(pointLight3.position - input.worldPos);
	float pointLightAmount3 = saturate(dot(input.normal, dirToPointLight3));

	float3 dirToPointLight4 = normalize(pointLight4.position - input.worldPos);
	float pointLightAmount4 = saturate(dot(input.normal, dirToPointLight4));


	// SpotLight calculation
	float angleFromCenter1 = max(dot(dirToPointLight1, spotLight1.direction), 0.0f);
	float spotAmount1 = pow(angleFromCenter1, spotLight1.spotPower);

	float angleFromCenter2 = max(dot(dirToPointLight2, spotLight2.direction), 0.0f);
	float spotAmount2 = pow(angleFromCenter2, spotLight2.spotPower);

	// Specular (for point light)
	float3 toCamera = normalize(CameraPosition - input.worldPos);
	float3 reflection = reflect(-dirToPointLight3, input.normal);
	float spec = pow(max(dot(reflection, toCamera), 0), 32);

	float3 toCamera1 = normalize(CameraPosition - input.worldPos);
	float3 reflection1 = reflect(-dirToPointLight4, input.normal);
	float spec1 = pow(max(dot(reflection1, toCamera1), 0), 32);


	// Total directional Light
	float3 totalDirLight = DirLightColor * dirLightAmount * textureColor;

	// Total point Light
	float3 totalPointLight = (pointLight3.color * pointLightAmount3 * textureColor) + (pointLight4.color * pointLightAmount4 * textureColor);

	// Total Spotlight
	float3 totalSpotLight = (spotAmount1 * pointLight1.color * textureColor) + (spotAmount2 * pointLight2.color * textureColor);

	float3 totalLight = totalPointLight + totalSpotLight + spec + spec1;

	return float4(totalLight, 1.0f);
}