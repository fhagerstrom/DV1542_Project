Texture2D Texture : register(t0);
Texture2D NormalMap : register (t1);
SamplerState Sampler : register(s0);

struct PointLight
{
	float4 colour;
	float3 position;
};

struct SpotLight
{
	float3 direction;
	float spotPower;
};

cbuffer lightData : register(b0)
{
	float4 directionalLightColour;
	float3 lightDirection;

	// Point light sources.
	PointLight pointLight1;
	PointLight pointLight2;

	PointLight pointLight3;
	PointLight pointLight4;

	SpotLight spotLight1;
	SpotLight spotLight2;


	float3 cameraPosition;
};

struct VertexToPixel
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 worldPos : POSITION;
	float2 uv : TEXCOORD;
};

float4 main(VertexToPixel input) : SV_TARGET
{
	input.normal = normalize(input.normal);
	input.tangent = normalize(input.tangent);

	float3 normalFromMap = NormalMap.Sample(Sampler, input.uv).xyz * 2 - 1;

	// Transform tangent to world space.
	float3 n = input.normal;
	float3 t = normalize(input.tangent - n * dot(input.tangent, n));
	float3 b = cross(t, n);

	float3x3 tbn = float3x3(t, b, n);
	input.normal = normalize(mul(normalFromMap, tbn));

	// Sample the texture.
	float4 textureColour = Texture.Sample(Sampler, input.uv);

	// Directional light.
	float directionalLightAmount = saturate(dot(input.normal, -normalize(lightDirection)));

	// Point lights.
	float3 directionToPointLight1 = normalize(pointLight1.position - input.worldPos);
	float pointLightAmount1 = saturate((dot(input.normal, directionToPointLight1)));

	float3 directionToPointLight2 = normalize(pointLight1.position - input.worldPos);
	float pointLightAmount2 = saturate((dot(input.normal, directionToPointLight2)));

	float3 directionToPointLight3 = normalize(pointLight1.position - input.worldPos);
	float pointLightAmount3 = saturate((dot(input.normal, directionToPointLight3)));

	float3 directionToPointLight4 = normalize(pointLight1.position - input.worldPos);
	float pointLightAmount4 = saturate((dot(input.normal, directionToPointLight4)));

	// Spotlights.
	float angleFromCenter1 = max(dot(directionToPointLight1, spotLight1.direction), 0.0f);
	float spotAmount1 = pow(angleFromCenter1, spotLight1.spotPower);

	float angleFromCenter2 = max(dot(directionToPointLight2, spotLight2.direction), 0.0f);
	float spotAmount2 = pow(angleFromCenter2, spotLight2.spotPower);

	// Specular.
	float3 toCamera = normalize(cameraPosition - input.worldPos);
	float3 reflection = reflect(-directionToPointLight3, input.normal);
	float specular = pow(max(dot(reflection, toCamera), 0), 32);

	float3 toCamera1 = normalize(cameraPosition - input.worldPos);
	float3 reflection1 = reflect(-directionToPointLight4, input.normal);
	float specular1 = pow(max(dot(reflection1, toCamera1), 0), 32);


	// Total Directional Light.
	float3 totalDirLight = directionalLightColour * directionalLightAmount * textureColour;

	// Total point Light.
	float3 totalPointLight = (pointLight3.colour * pointLightAmount3 * textureColour) + (pointLight4.colour * pointLightAmount4 * textureColour);

	// Total Spotlight.
	float3 totalSpotLight = (spotAmount1 * pointLight1.colour * textureColour) + (spotAmount2 * pointLight2.colour * textureColour);

	float3 totalLight = totalPointLight + totalSpotLight + specular + specular1;

	return float4(totalLight, 1.0f);
}