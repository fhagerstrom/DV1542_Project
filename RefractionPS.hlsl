Texture2D shaderTexture : register(t0);
Texture2D NormalMap : register(t1);
TextureCube Sky : register(t2);
SamplerState Sampler : register(s0);

cbuffer lightData : register(b0)
{
	float4 dirLightColor;
	float3 dirLightDirection;

	float4 pointLightColor;
	float3 pointLightPosition;

	float3 cameraPosition;
}

struct VertexToPixel
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 worldPos : POSITION;
	float2 uv : TEXCOORD;
	float clip : SV_ClipDistance0;
};

float4 main(VertexToPixel input) : SV_TARGET
{
	input.normal = normalize(input.normal);
	input.tangent = normalize(input.tangent);

	// Normal mapping.
	float3 normalFromMap = NormalMap.Sample(Sampler, input.uv).rgb * 2 - 1;
	
	// Calculate TBN matrix.
	float3 n = input.normal;
	float3 t = normalize(input.tangent - n * dot(input.tangent, n));
	float3 b = cross(t, n);
	float3x3 tbn = float3x3(t, b, n);

	// Directional light calculation.
	// Figure out direction to the light.
	float3 dirToPointLight = normalize(pointLightPosition - input.worldPos);
	float pointLightAmount = saturate(dot(input.normal, dirToPointLight));

	// Specular calculation.
	float3 toCamera = normalize(cameraPosition - input.worldPos);
	float3 reflection = reflect(-dirToPointLight, input.normal);
	float spec = pow(max(dot(reflection, toCamera), 0), 32);

	// Sample the texture.
	float4 textureColor = shaderTexture.Sample(Sampler, input.uv);

	// Calculate the reflection vector from the camera
	// bouncing off the surface.
	float4 skycolor = Sky.Sample(Sampler, reflect(-toCamera, input.normal));

	return lerp(textureColor, skycolor, 0.5f);

	return textureColor;
}