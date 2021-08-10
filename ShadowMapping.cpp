#include "ShadowMapping.h"

// For the DirectX Math library

ShadowMapping::ShadowMapping()
{

}

ShadowMapping::ShadowMapping(ID3D11Device* device, UINT size) : depthMapSRV(nullptr), depthMapDSV(nullptr)
{
	// TEXTURE2D
	// We are using typeless format since the dsv is going to interpret 
	// bits as DXGI_FORMATD24UNORM_S8UINT where SRV is going
	// to interpret bits as DXGI_FORMAT_R24_UNORM_X8_TYPELESS.

	D3D11_TEXTURE2D_DESC textureDesc;

	textureDesc.Width = size;
	textureDesc.Height = size;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	
	
	ID3D11Texture2D* depthMap = 0;
	HRESULT(hr) = (device -> CreateTexture2D(&textureDesc, 0, &depthMap));
	if (FAILED(hr))
	{			
		std::cout << "FAILED TO CREATE SHADOW TEXTURE2D!" << std::endl;
		return;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
	depthStencilViewDesc.Flags = 0;
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	hr = (device->CreateDepthStencilView(depthMap, &depthStencilViewDesc, &depthMapDSV));

	if (FAILED(hr))
	{
		std::cout << "FAILED TO CREATE SHADOW DEPTH STENCIL VIEW" << std::endl;
		return;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC shadeResourceViewDesc;
	shadeResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	shadeResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shadeResourceViewDesc.Texture2D.MipLevels = textureDesc.MipLevels;
	shadeResourceViewDesc.Texture2D.MostDetailedMip = 0;

	hr = (device->CreateShaderResourceView(depthMap, &shadeResourceViewDesc, &depthMapSRV));
	if ((FAILED(hr)))
	{
		std::cout << "FAILED TO CREATE SHADOW SHADER RESOURCE VIEW" << std::endl;
		return;
	}
	depthMap->Release(); //view saves reference so we dont need this anymore
}

ShadowMapping::~ShadowMapping() 
{
	this->depthMapSRV->Release(); this->depthMapDSV->Release();
}


ID3D11DepthStencilView** ShadowMapping::GetDSV()
{ 
	return &this->depthMapDSV; 
}

	
ID3D11ShaderResourceView** ShadowMapping::GetSRV()
{ 
	return &this->depthMapSRV; 
}

void ShadowMapping::BindShadowMap(ID3D11DeviceContext* context)
{
	
		// Set viewport and shader resource to specific
		// shader resource that depth texture will be written to
		ID3D11ShaderResourceView* null = nullptr;
		context->PSSetShaderResources(3, 1, &null);
		// context->RSSetViewports(1, &viewport);
		// Set null render target because we  only draw to depth buffer. 

		// null render target, this will disable color writes.
		ID3D11RenderTargetView* renderTargets[1] = { 0 };
		// context->OMSetRenderTargets(1, renderTargets, depthMapDSV);
		// context->ClearDepthStencilView(depthMapDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

}
