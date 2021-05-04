#pragma once

#include <DirectXMath.h>

// Custom vertex definition
struct Vertex
{
	Vertex() 
	{

	}

	Vertex(float x, float y, float z, float u, float v, float nx, float ny, float nz) : position(x, y, z), uv(u, v), normal(nx, ny, nz) 
	{

	}

	DirectX::XMFLOAT3 position;	    // The position of the vertex
	DirectX::XMFLOAT2 uv;           // uv Coordinate for texturing
	DirectX::XMFLOAT3 normal;       // normal for lighting
	DirectX::XMFLOAT3 tangent;		// For normal mapping
	DirectX::XMFLOAT4 colour;		// Terrain related
};