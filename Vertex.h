#pragma once

#include <DirectXMath.h>
namespace dx = DirectX;

// Vertex structure - Supports shaders and textures.
struct Vertex
{
	dx::XMFLOAT3 position;
	dx::XMFLOAT2 uv;
	dx::XMFLOAT3 normal;

	// dx::XMFLOAT3 tangent;
};