#include "ShadowMapping.h"

ShadowMapping::ShadowMapping()
{
}

ShadowMapping::~ShadowMapping()
{
}

bool ShadowMapping::CreateDepthStencil(ID3D11Device* device, int windowWidth, int windowHeight)
{
	// CREATE TEXTURE 2D ARRAY
	D3D11_TEXTURE2D_DESC textureDesc;
	textureDesc.Width = windowWidth;
	textureDesc.Height = windowHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = nrOfLights;
	textureDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	if (FAILED(device->CreateTexture2D(&textureDesc, nullptr, &dsTexture)))
	{
		MessageBox(NULL, L"Couldn't create texture2d array for shadows!", L"SHAODW ERROR", MB_OK);
		return false;
	}

	// DEPTH STENCIL ARRAY
	D3D11_DEPTH_STENCIL_VIEW_DESC descView = {};
	descView.Format = DXGI_FORMAT_D32_FLOAT;
	descView.Flags = 0;
	descView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	descView.Texture2DArray.ArraySize = 1;
	descView.Texture2DArray.MipSlice = 0;

	for (int i = 0; i < nrOfLights; i++) 
	{
		descView.Texture2DArray.FirstArraySlice = D3D11CalcSubresource(0, i, 1);
		ID3D11DepthStencilView* dsView;
		if (FAILED(device->CreateDepthStencilView(dsTexture, &descView, &dsView))) 
		{
			MessageBox(NULL, L"Couldn't create depth stencil view for shadows!", L"SHAODW ERROR", MB_OK);
			return false;
		}
		dsViews.push_back(dsView);
	}

	// DEPTH STENCIL TO SRV
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Texture2DArray.ArraySize = nrOfLights;
	srvDesc.Texture2DArray.MostDetailedMip = 0;
	srvDesc.Texture2DArray.MipLevels = 1;

	HRESULT hr = device->CreateShaderResourceView(dsTexture, &srvDesc, &shadowSRV);
	if (FAILED(hr)) 
	{
		MessageBox(NULL, L"Couldn't create shadow SRV!", L"SHADOW ERROR", MB_OK);
		return false;
	}
	return true;
}
