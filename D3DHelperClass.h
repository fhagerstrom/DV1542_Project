#pragma once

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <directxmath.h>

namespace dx = DirectX;

class D3DHelperClass
{
public:
	D3DHelperClass();
	D3DHelperClass(const D3DHelperClass&);
	~D3DHelperClass();

	bool Initialize(int screenWidth, int screenHeight, HWND hwnd, bool fullscreen, float screenDepth, float screenNear);
	void Shutdown();
	
	void BeginScene(float, float, float, float);
	void EndScene();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	void GetProjectionMatrix(dx::XMMATRIX&);
	void GetWorldMatrix(dx::XMMATRIX&);
	void GetOrthoMatrix(dx::XMMATRIX&);

	void TurnZBufferOn();
	void TurnZBufferOff();

	void TurnOnAlphaBlending();
	void TurnOffAlphaBlending();

	ID3D11DepthStencilView* GetDepthStencilView();
	void SetBackBufferRenderTarget();
	void ResetViewport();

private:

	
	IDXGISwapChain* swapChain;
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	ID3D11RenderTargetView* renderTargetView;
	ID3D11Texture2D* depthStencilBuffer;
	ID3D11DepthStencilState* depthStencilState;
	ID3D11DepthStencilView* depthStencilView;
	ID3D11RasterizerState* rasterState;
	D3D11_VIEWPORT viewport;

	dx::XMMATRIX projectionMatrix;
	dx::XMMATRIX worldMatrix;
	dx::XMMATRIX orthoMatrix;

	ID3D11DepthStencilState* depthDisabledStencilState;
	ID3D11BlendState* alphaEnableBlendingState;
	ID3D11BlendState* alphaDisableBlendingState;
};

