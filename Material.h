#pragma once
#include "SimpleShader.h"

class Material
{
private:
	SimpleVertexShader* vShader;
	SimplePixelShader* pShader;
	ID3D11ShaderResourceView* matSRV;
	ID3D11ShaderResourceView* matNormal;
	ID3D11SamplerState* matSampler;

public:
	Material(SimpleVertexShader* vertexShader, SimplePixelShader* pixelShader, ID3D11ShaderResourceView* materialSRV, 
			ID3D11ShaderResourceView* materialNormal, ID3D11SamplerState* materialSampler);
	~Material();
	ID3D11ShaderResourceView* GetTextureSRV();
	ID3D11ShaderResourceView* GetNormalMapSRV();
	ID3D11SamplerState* GetSamplerState();
};