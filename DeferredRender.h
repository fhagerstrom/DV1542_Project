#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "DxHandler.h"

using namespace DirectX;

// Defining amount of buffers to store the data to for deferred rendering.
#define BUFFER_COUNT 8 // Define buffer count to 8,
					   // does not matter performance-wise if you use all spots even if they're not in use

// Handle the geometry buffer for the deferred pipeline
// as well as defining texture, render target view and shader resource view arrays.

class DeferredRender
{
public:
	DeferredRender();
	~DeferredRender();
	bool Initialize(ID3D11Device* device, DxHandler* dxHandler, int windowWidth, int windowHeight);
	void Shutdown();

	void SetRenderTargets(ID3D11DeviceContext* context);
	void ClearRenderTargets(ID3D11DeviceContext* context, float r, float g, float b, float a);

	ID3D11DepthStencilView* GetDSV() { return depthStencilView; }
	/*ID3D11Texture2D* GetTextureArrays() { return renderTargetTextureArray[BUFFER_COUNT]; }
	ID3D11RenderTargetView* GetRTVArray() { return renderTargetViewArray[BUFFER_COUNT]; }
	ID3D11ShaderResourceView* GetSRVArray() { return shaderResourceViewArray[BUFFER_COUNT]; }*/

	// Declare texture, RTV and SRV arrays for the deferred buffers.
	// These are public to be able to access them in the shander handler.
	ID3D11Texture2D* renderTargetTextureArray[BUFFER_COUNT];
	ID3D11RenderTargetView* renderTargetViewArray[BUFFER_COUNT];
	ID3D11ShaderResourceView* shaderResourceViewArray[BUFFER_COUNT];

	ID3D11Texture2D* depthStencilBuffer;
	ID3D11Texture2D* backBuffer;
	ID3D11DepthStencilView* depthStencilView;
	ID3D11UnorderedAccessView* deferredUav;
	D3D11_VIEWPORT viewport;

private:
	int windowWidth, windowHeight;

	HRESULT hr;
	DxHandler* dxHandler;
};