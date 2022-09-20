#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

// Defining amount of buffers to store the data to for deferred rendering.
#define BUFFER_COUNT 3 // Change the buffer count successively as we add more stuff to the arrays.

// Handle the geometry buffer for the deferred pipeline
// as well as defining texture, render target view and shader resource view arrays.

class DeferredRender
{
public:
	DeferredRender();
	~DeferredRender();
	bool Initialize(ID3D11Device* device, int windowWidth, int windowHeight, int screenDepth, int screenNear);
	void Shutdown();

	void SetRenderTargets(ID3D11DeviceContext* context);
	void ClearRenderTargets(ID3D11DeviceContext* context, float r, float g, float b, float a);

	ID3D11ShaderResourceView* GetShaderResourceView(int view) { return this->shaderResourceViewArray[view]; }

private:
	int windowWidth, windowHeight;

	HRESULT hr;

	// Declare texture, RTV and SRV arrays for the deferred buffers
	ID3D11Texture2D* renderTargetTextureArray[BUFFER_COUNT];
	ID3D11RenderTargetView* renderTargetViewArray[BUFFER_COUNT];
	ID3D11ShaderResourceView* shaderResourceViewArray[BUFFER_COUNT];

	ID3D11Texture2D* depthStencilBuffer;
	ID3D11DepthStencilView* depthStencilView;
	D3D11_VIEWPORT viewport;
};