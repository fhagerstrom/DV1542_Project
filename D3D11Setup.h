#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <iostream>

bool SetupD3D11(const UINT width, const UINT height, HWND window, ID3D11Device*& device, ID3D11DeviceContext*& immediateContext,
	ID3D11RenderTargetView*& rtv, ID3D11Texture2D*& dsTexture, ID3D11DepthStencilView*& dsView, IDXGISwapChain*& swapChain, D3D11_VIEWPORT& viewport);