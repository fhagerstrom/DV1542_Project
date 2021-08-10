#pragma once
#include <d3d11.h>
#include <iostream>

class ShadowMapping
{
private:
	ID3D11ShaderResourceView* depthMapSRV;
	ID3D11DepthStencilView* depthMapDSV;
	D3D11_VIEWPORT viewport;
public:
	ShadowMapping();
	ShadowMapping(ID3D11Device* device, UINT size);
	~ShadowMapping();
	
	ID3D11DepthStencilView** GetDSV();
	ID3D11ShaderResourceView** GetSRV();
	void BindShadowMap(ID3D11DeviceContext* context);
};

