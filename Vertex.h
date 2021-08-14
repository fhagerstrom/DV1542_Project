#pragma once

#include <DirectXMath.h>

using namespace DirectX;

struct Vertex
{
	XMFLOAT3 position;
	XMFLOAT2 texture;
	XMFLOAT3 normal;
	XMFLOAT3 tangent;
	XMFLOAT3 binormal;
};