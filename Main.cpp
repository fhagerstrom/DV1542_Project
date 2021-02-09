#include <Windows.h>
#include <iostream>
#include <d3d11.h>
#include <chrono>

#include "WindowSetup.h"
#include "D3D11Setup.h"


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	// Set window size and window handler
	const UINT WIDTH = 1024;
	const UINT HEIGHT = 768;
	HWND window;

	// Check if Window handler failed
	if (!SetWindow(hInstance, WIDTH, HEIGHT, nCmdShow, window))
	{
		std::cerr << "Window setup failed!" << std::endl;
		return -1;
	}

	// Initialise DirectX objects
	ID3D11Device* device;
	ID3D11DeviceContext* immediateContext;
	ID3D11RenderTargetView* rtv;
	ID3D11Texture2D* dsTexture;
	ID3D11DepthStencilView* dsView;
	IDXGISwapChain* swapChain;
	D3D11_VIEWPORT viewport;
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* inputLayout;

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* constantBuffer;
	ID3D11Buffer* constBufferLight;

	ID3D11Texture2D* texture;
	ID3D11ShaderResourceView* textureSRV;
	ID3D11SamplerState* sampler;

	// Check if the D3D11 setup failed
	if (!SetupD3D11(WIDTH, HEIGHT, window, device, immediateContext, rtv, dsTexture, dsView, swapChain, viewport))
	{
		std::cerr << "DirectX setup failed!" << std::endl;
		return -1;
	}

	MSG msg = {};

	// As long as you don't press esc or close the window...
	while (!(GetKeyState(VK_ESCAPE) & 0x8000) && msg.message != WM_QUIT)
	{
		// PeekMessage, if there's a queue to be sent to the MSG structure
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// @TODO: Do all the other stuff


	}
}
