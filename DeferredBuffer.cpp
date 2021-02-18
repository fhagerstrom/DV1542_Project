#include "DeferredBuffer.h"

DeferredRendering::DeferredRendering()
{
	for(int i = 0; i < buffer_count; i++)
	{
		renderTargetTextureArray[i] = 0;
		renderTargetViewArray[i] = 0;
		shaderResourceViewArray[i] = 0;
	}
	depthStencilBuffer = 0;
	depthStencilView = 0;
	textureHeight = 0;
	textureWidth = 0;

	viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = static_cast<float>(textureWidth);
    viewport.Height = static_cast<float>(textureHeight);
    viewport.MinDepth = 0;
    viewport.MaxDepth = 1;
}

DeferredRendering::DeferredRendering(const DeferredRendering& other)
{
	
}

DeferredRendering::~DeferredRendering()
{

}

//device, texture width, texture heigh, screen depth, screen near
bool DeferredRendering::Initialize(ID3D11Device* device, int width, int height, float depth, float near)
{
	D3D11_TEXTURE2D_DESC textureDescription;
	HRESULT result;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDescription;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDescription;
	D3D11_TEXTURE2D_DESC depthBufferDescription;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDescription;

	//save variables to render
	textureHeight = height;
	textureWidth = width;

	//setup render target desc.
	ZeroMemory(&textureDescription, sizeof(textureDescription));
	textureDescription.Height = textureHeight;
	textureDescription.Width = textureWidth;
	textureDescription.MipLevels = 1;
	textureDescription.ArraySize = 1;
	textureDescription.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDescription.SampleDesc.Count = 1;
	textureDescription.Usage = D3D11_USAGE_DEFAULT;
	textureDescription.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDescription.CPUAccessFlags = 0;
	textureDescription.MiscFlags = 0;

	//create array of render target textures. This will be written to by the deferred shader
	for(int i = 0; i < buffer_count; i++)
	{
		result = device -> CreateTexture2D(&textureDescription, NULL, &renderTargetTextureArray[i]);
		if(FAILED(result))
		{
			std::cerr << "ERROR! Result failed in deferredRendering while creating render target textures!" << std::endl; 
			return false;
		}
	}
	
	//time to do same thing for render target view
	renderTargetViewDescription.Format = textureDescription.Format;
	renderTargetViewDescription.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDescription.Texture2D.MipSlice = 0;

	for(int i = 0; i < buffer_count; i++)
	{
		result = device -> CreateRenderTargetView(renderTargetTextureArray[i], &renderTargetViewDescription, &renderTargetViewArray[i]);
		if(FAILED(result))
		{
			std::cerr << "ERROR! Result failed in deferredRendering while creating render target view!" << std::endl; 
			return false;
		}
	}

	//setup shader resource views
	shaderResourceViewDescription.Format = textureDescription.Format;
	shaderResourceViewDescription.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDescription.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDescription.Texture2D.MipLevels = 1;

	for(int i = 0; i < buffer_count; i++)
	{
		result = device -> CreateShaderResourceView(renderTargetTextureArray[i], &shaderResourceViewDescription, &shaderResourceViewArray[i]);
		if(FAILED(result))
		{
			std::cerr << "ERROR! Result failed in deferredRendering while creating shader resource view!" << std::endl; 
			return false;
		}
	}


	//setup depth buffer desc
	ZeroMemory(&depthBufferDescription, sizeof(depthBufferDescription));

	depthBufferDescription.Width = textureWidth;
	depthBufferDescription.Height = textureHeight;
	depthBufferDescription.MipLevels = 1;
	depthBufferDescription.ArraySize = 1;
	depthBufferDescription.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDescription.SampleDesc.Count = 1;
	depthBufferDescription.SampleDesc.Quality = 0;
	depthBufferDescription.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDescription.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDescription.CPUAccessFlags = 0;
	depthBufferDescription.MiscFlags = 0;

	result = device -> CreateTexture2D(&depthBufferDescription, NULL, &depthStencilBuffer);
	if(FAILED(result))
	{
		std::cerr << "ERROR! Result failed in deferredRendering while creating depth buffer desc!" << std::endl; 
		return false;
	}


	//setup depth stencil view desc
	ZeroMemory(&depthStencilViewDescription, sizeof(depthStencilViewDescription));

	depthStencilViewDescription.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDescription.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDescription.Texture2D.MipSlice = 0;

	result = device->CreateDepthStencilView(depthStencilBuffer, &depthStencilViewDescription, &depthStencilView);
	if(FAILED(result))
	{
		std::cerr << "ERROR! Result failed in deferredRendering while creating depth stencil view desc!" << std::endl; 
		return false;
	}

	//finally setup viewport
	viewport.Width = (float)textureWidth;
	viewport.Height = (float)textureHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	return true;
}

void DeferredRendering::ReleaseStuff()
{
	if(depthStencilView)
	{
		depthStencilView->Release();
		depthStencilView = 0;
	}
	if(depthStencilBuffer)
	{
		depthStencilBuffer->Release();
		depthStencilBuffer = 0;
	}
	for(int i = 0; i < buffer_count; i++)
	{
		if(shaderResourceViewArray[i])
		{
			shaderResourceViewArray[i]->Release();
			shaderResourceViewArray[i] = 0;
		}

		if(renderTargetViewArray[i])
		{
			renderTargetViewArray[i]->Release();
			renderTargetViewArray[i] = 0;
		}

		if(renderTargetTextureArray[i])
		{
			renderTargetTextureArray[i]->Release();
			renderTargetTextureArray[i] = 0;
		}
	}
}

void DeferredRendering::SetRenderTargets(ID3D11DeviceContext* deviceContext)
{
	deviceContext->OMSetRenderTargets(buffer_count, renderTargetViewArray, depthStencilView);

	deviceContext->RSSetViewports(1, &viewport);
}

//	deviceContext, red, green, blue, alpha
void DeferredRendering::ClearRenderTargets(ID3D11DeviceContext* deviceContext, float r, float g, float b, float a)
{
	float color[4];

	// Setup color 
	color[0] = r;
	color[1] = g;
	color[2] = b;
	color[3] = a;

	// Clear render target buffers.
	for(int i = 0; i < buffer_count; i++)
	{
		deviceContext->ClearRenderTargetView(renderTargetViewArray[i], color);
	}
	// Clear depth buffer.
	deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

}

ID3D11ShaderResourceView* DeferredRendering::GetShaderResourceView(int view)
{
	return shaderResourceViewArray[view];
}