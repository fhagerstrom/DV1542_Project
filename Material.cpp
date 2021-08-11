#include "Material.h"


Material::Material(SimpleVertexShader* _vertexShader, SimplePixelShader* _pixelShader, ID3D11ShaderResourceView* _materialSRV, ID3D11ShaderResourceView* _normalSRV, ID3D11SamplerState* _materialSampler)
{
	vertexShader = _vertexShader;
	pixelShader = _pixelShader;
	materialSRV = _materialSRV;
	normalSRV = _normalSRV;
	materialSampler = _materialSampler;
}

Material::~Material()
{
}

SimpleVertexShader* Material::GetVertexShader()
{
	return vertexShader;
}

SimplePixelShader* Material::GetPixelShader()
{
	return pixelShader;
}

ID3D11ShaderResourceView* Material::GetMaterialSRV()
{
	return materialSRV;
}

ID3D11ShaderResourceView* Material::GetNormalSRV()
{
	return normalSRV;
}

ID3D11SamplerState* Material::GetMaterialSampler()
{
	return materialSampler;
}