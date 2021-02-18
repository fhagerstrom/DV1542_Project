#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <iostream>
const int buffer_count = 2;

class DeferredRendering
{
private:
	int textureWidth, textureHeight;

	//buffer count is 2
	ID3D11Texture2D* renderTargetTextureArray[buffer_count];
	ID3D11RenderTargetView* renderTargetViewArray[buffer_count];
	ID3D11ShaderResourceView* shaderResourceViewArray[buffer_count];
	D3D11_VIEWPORT viewport;
	ID3D11DepthStencilView* depthStencilView;
	ID3D11Texture2D* depthStencilBuffer;



public:

	DeferredRendering();
	DeferredRendering(const DeferredRendering& other);
	~DeferredRendering();

	//device, texture width, texture heigh, screen depth, screen near
	bool Initialize(ID3D11Device* device, int width, int height, float depth, float near);
	void ReleaseStuff();

	void SetRenderTargets(ID3D11DeviceContext* deviceContext);
	//	deviceContext, red, green, blue, alpha
	void ClearRenderTargets(ID3D11DeviceContext* deviceContext, float r, float g, float b, float a);

	ID3D11ShaderResourceView* GetShaderResourceView(int view);
};

