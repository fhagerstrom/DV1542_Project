cbuffer lightData : register(b0)
{
    float4 DirLightColor;
    float3 DirLightDirection;

    float4 PointLightColor;
    float3 PointLightPosition;

    float3 CameraPosition;
}

cbuffer waterData : register(b1)
{
    float waterTranslation;
}

//External texture-related data
TextureCube reflectionTexture : register(t0);
Texture2D refractionTexture : register(t1);
Texture2D NormalMap : register(t2);
SamplerState Sampler : register(s0);

struct VertexToPixel
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float4 worldPos : POSITION;
    float2 uv : TEXCOORD;
    float3 reflWorldPos : POSITION1;
};

float4 main(VertexToPixel input) : SV_TARGET
{
	// Move the position the water normal is sampled from to simulate moving water.	
    input.uv.x += waterTranslation;


    input.normal = normalize(input.normal);
    input.tangent = normalize(input.tangent);

	//Normal Mapping
    float3 normalFromMap = NormalMap.Sample(Sampler, input.uv).rgb * 2 - 1;

	//Calculate my TBN matrix to get the normal into world space
    float3 N = input.normal;
    float3 T = normalize(input.tangent - N * dot(input.tangent, N));
    float3 B = cross(T, N);
    float3x3 TBN = float3x3(T, B, N);

    input.normal = normalize(mul(normalFromMap, TBN));

	//DirectionalLight calculation
    float dirLightAmount = saturate(dot(input.normal, -normalize(DirLightDirection)));

	//Point light calculation
    float3 dirToPointLight = normalize(PointLightPosition - input.worldPos);
    float pointLightAmount = saturate(dot(input.normal, dirToPointLight));

	//Specular
    float3 toCamera = normalize(CameraPosition - input.worldPos);
    float3 refl = reflect(-dirToPointLight, input.normal);
    float spec = pow(max(dot(refl, toCamera), 0), 32);

    float3 toCamera1 = normalize(CameraPosition - input.reflWorldPos);
	//Sample the texture
    float4 reflTextureColor = reflectionTexture.Sample(Sampler, reflect(-toCamera1, input.normal));


	//calculate the projected refraction coordinates
    float2 refractionTexCoord;
    refractionTexCoord.x = input.worldPos.x / input.worldPos.w / 2.0 + 0.5f;
    refractionTexCoord.y = -input.worldPos.y / input.worldPos.w / 2.0f + 0.5f;

	// Re-position the texture coordinate sampling position by the normal map value to simulate the rippling wave effect.
    refractionTexCoord = refractionTexCoord + (normalFromMap.xy * 0.03);


	//Sample refraction texture
    float4 refractionTextureColor = refractionTexture.Sample(Sampler, refractionTexCoord);

	//Calculation of fresnal factor
    float r = (1.2f - 1.0f) / (1.2f + 1.0f);
    float fresnelFactor = max(0.0f, min(1.0f, r + (1.0f - r) * pow(1.0f - dot(normalFromMap, toCamera), 2)));

	//return lerp(reflTextureColor, skyColor, 0.6f);
    float4 color = lerp(reflTextureColor, refractionTextureColor, 0.5);

    return color;
}