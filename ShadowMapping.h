#pragma once
#include <d3d11.h>
#include <vector>
#include "Light.h"

class ShadowMapping
{
public:
	ShadowMapping();
	~ShadowMapping();

private:
	bool CreateDepthStencil(ID3D11Device* device, int windowWidth, int windowHeight);

	int nrOfLights;
	Light* spotLight;

	ID3D11Texture2D* dsTexture;
	std::vector<ID3D11DepthStencilView*> dsViews;
	ID3D11Resource* shadowResource;
	ID3D11ShaderResourceView* shadowSRV;
};