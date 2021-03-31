#include "Material.h"

Material::Material(SimpleVertexShader* vertexShader, SimplePixelShader* pixelShader, ID3D11ShaderResourceView* materialSRV, 
			ID3D11ShaderResourceView* materialNormal, ID3D11SamplerState* materialSampler)
{
	vShader = vertexShader;
	pShader = pixelShader;
	matSRV = materialSRV;
	matNormal = materialNormal;
	matSampler = materialSampler;
}

Material::~Material()
{

}

ID3D11ShaderResourceView* Material::GetTextureSRV()
{
	return matSRV;;
}

ID3D11ShaderResourceView* Material::GetNormalMapSRV()
{
	return matNormal;
}

ID3D11SamplerState* Material::GetSamplerState()
{
	return matSampler;
}


