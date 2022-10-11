#pragma once

#include <DirectXMath.h>

using namespace DirectX;

struct Vertex
{
	Vertex() {}
	Vertex(float x, float y, float z,
		float u, float v,
		float nx, float ny, float nz)
		: position(x, y, z), texCoord(u, v), normal(nx, ny, nz) { }

	XMFLOAT3 position;
	XMFLOAT2 texCoord;
	XMFLOAT3 normal;
};