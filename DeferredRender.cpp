#include "DeferredRender.h"

DeferredRender::DeferredRender()
{
	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		renderTargetTextureArray[i] = 0;
		renderTargetViewArray[i] = 0;
		shaderResourceViewArray[i] = 0;
	}

	depthStencilBuffer = 0;
	depthStencilView = 0;
}

DeferredRender::~DeferredRender()
{
}

bool DeferredRender::Initialize(ID3D11Device* device, DxHandler* dxHandler, int windowWidth, int windowHeight)
{
	this->dxHandler = dxHandler;

	D3D11_TEXTURE2D_DESC textureDesc;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;

	this->windowWidth = windowWidth;
	this->windowHeight = windowHeight;

	// Initialize the render target texture description.
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	// Setup the render target texture description.
	textureDesc.Width = windowWidth;
	textureDesc.Height = windowHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	// Create the render target textures.
	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		hr = device->CreateTexture2D(&textureDesc, NULL, &renderTargetTextureArray[i]);
		if (FAILED(hr))
		{
			MessageBox(nullptr, L"Couldn' create the render target texture array!", L"DEFERRED BUFFER ERROR", MB_OK);
			return false;
		}
	}

	// Setup the description of the render target view.
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Create the render target views.
	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		hr = device->CreateRenderTargetView(renderTargetTextureArray[i], &renderTargetViewDesc, &renderTargetViewArray[i]);
		if (FAILED(hr))
		{
			return false;
		}
	}

	// Setup the description of the shader resource view.
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// Create the shader resource views.
	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		hr = device->CreateShaderResourceView(renderTargetTextureArray[i], &shaderResourceViewDesc, &shaderResourceViewArray[i]);
		if (FAILED(hr))
		{
			MessageBox(NULL, L"Couldn't create SRVs for deferred!", L"DEFERRED ERROR", MB_OK);
			return false;
		}
	}

	// Initialize the description of the depth buffer.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// Set up the description of the depth buffer.
	depthBufferDesc.Width = windowWidth;
	depthBufferDesc.Height = windowHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// Create the texture for the depth buffer using the filled out description.
	hr = device->CreateTexture2D(&depthBufferDesc, NULL, &depthStencilBuffer);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Couldnt create Texture2D object for deferred!", L"DEFERRED ERROR", MB_OK);
		return false;
	}

	// Initailze the depth stencil view description.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// Set up the depth stencil view description.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	hr = device->CreateDepthStencilView(depthStencilBuffer, &depthStencilViewDesc, &depthStencilView);
	if (FAILED(hr))
	{
		return false;
	}

	// Set UAV description for deferred.
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;

	// Get the pointer to the back buffer.
	/*hr = dxHandler->GetSwapChain()->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Couldn't get back buffer for deferred!", L"DEFERRED ERROR", MB_OK);
		return false;
	}*/

	HRESULT hr = dxHandler->GetDevice()->CreateUnorderedAccessView(dxHandler->GetBackBuffer(), &uavDesc, &deferredUav);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Couldn't create Unordered access view for deferred!", L"DEFERRED ERROR", MB_OK);
		return false;
	}

	// Setup the viewport for rendering.
	viewport.Width = (float)windowWidth;
	viewport.Height = (float)windowHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	return true;
}

void DeferredRender::Shutdown()
{
	depthStencilView->Release();
	depthStencilView = 0;

	depthStencilBuffer->Release();
	depthStencilBuffer = 0;

	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		renderTargetTextureArray[i]->Release();
		renderTargetTextureArray[i] = 0;

		shaderResourceViewArray[i]->Release();
		shaderResourceViewArray[i] = 0;

		renderTargetViewArray[i]->Release();
		renderTargetViewArray[i] = 0;
	}
}

void DeferredRender::SetRenderTargets(ID3D11DeviceContext* context)
{
	// Bind the render target view array and depth stencil buffer to the output render pipeline.
	context->OMSetRenderTargets(BUFFER_COUNT, renderTargetViewArray, depthStencilView);

	// Set the viewport.
	context->RSSetViewports(1, &viewport);

}

void DeferredRender::ClearRenderTargets(ID3D11DeviceContext* context, float r, float g, float b, float a)
{
	float color[4];

	// Setup the color to clear the buffer to.
	color[0] = r;
	color[1] = g;
	color[2] = b;
	color[3] = a;

	// Clear the render target buffers.
	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		context->ClearRenderTargetView(renderTargetViewArray[i], color);
	}

	// Clear the depth buffer.
	context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

}
