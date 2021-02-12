#pragma once
#define ALIGN16 __declspec(align(16))

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <array>
#include <d3d11.h>
#include <DirectXMath.h>



bool SetupPipeline(ID3D11Device* device, ID3D11Buffer*& vertexBuffer, ID3D11VertexShader*& vertexShader, ID3D11PixelShader*& pixelShader, ID3D11InputLayout*& inputLayout,
	ID3D11Texture2D*& texture, ID3D11ShaderResourceView*& textureSRV, ID3D11SamplerState*& sampler, ID3D11Buffer*& constantBuffer, ID3D11Buffer*& constBufferLight);