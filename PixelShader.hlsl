// External texture-related data
Texture2D Texture : register(t0);
Texture2D NormalMap : register(t1);
TextureCube SkyMap : register(t2);
SamplerState Sampler : register(s0);

struct PointLight
{
	float4 colour;
	float3 Position;
};

struct SpotLight
{
	float3 direction;
	float spotPower;
};

cbuffer lightData : register(b0)
{
	float4 DirLightColour;
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
	float4 textureColour = Texture.Sample(Sampler, input.uv);

	// Directional light calculation
	float dirLightAmount = saturate(dot(input.normal, -normalize(DirLightDirection)));

	// Point light calculation
	float3 dirToPointLight1 = normalize(pointLight1.Position - input.worldPos);
	float pointLightAmount1 = saturate(dot(input.normal, dirToPointLight1));

	float3 dirToPointLight2 = normalize(pointLight2.Position - input.worldPos);
	float pointLightAmount2 = saturate(dot(input.normal, dirToPointLight2));

	float3 dirToPointLight3 = normalize(pointLight3.Position - input.worldPos);
	float pointLightAmount3 = saturate(dot(input.normal, dirToPointLight3));

	float3 dirToPointLight4 = normalize(pointLight4.Position - input.worldPos);
	float pointLightAmount4 = saturate(dot(input.normal, dirToPointLight4));


	// SpotLight calculation
	float angleFromCenter1 = max(dot(dirToPointLight1, spotLight1.direction), 0.0f);
	float spotAmount1 = pow(angleFromCenter1, spotLight1.spotPower);

	float angleFromCenter2 = max(dot(dirToPointLight2, spotLight2.direction), 0.0f);
	float spotAmount2 = pow(angleFromCenter2, spotLight2.spotPower);

	// Specular (for point light)
	float3 toCamera = normalize(CameraPosition - input.worldPos);
	float3 reflection = reflect(-dirToPointLight3, input.normal);
	float spec = pow(max(dot(reflection, toCamera), 0), 64);

	float3 toCamera1 = normalize(CameraPosition - input.worldPos);
	float3 reflection1 = reflect(-dirToPointLight4, input.normal);
	float spec1 = pow(max(dot(reflection1, toCamera1), 0), 64);


	// Total directional Light
	float3 totalDirLight = DirLightColour * dirLightAmount * textureColour;

	// Total point Light
	float3 totalPointLight = (pointLight3.colour * pointLightAmount3 * textureColour) + (pointLight4.colour * pointLightAmount4 * textureColour);

	// Total Spotlight
	float3 totalSpotLight = (spotAmount1 * pointLight1.colour * textureColour) + (spotAmount2 * pointLight2.colour * textureColour);

	float3 totalLight = totalPointLight + totalSpotLight + spec + spec1;

	// Calculate the reflection vector from the camera
	// bouncing off the surface
	/*float4 skyColor = Sky.Sample(Sampler, reflect(-toCamera, input.normal));
	return lerp(textureColor, skyColor, 0.5f);*/

	return float4(totalLight, 1.0f);
}