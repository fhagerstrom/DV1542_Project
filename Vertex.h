#pragma once

#include <DirectXMath.h>

// Custom vertex definition
struct Vertex
{
	Vertex() 
	{

	}

	Vertex(float x, float y, float z, float u, float v, float nx, float ny, float nz) : Position(x, y, z), UV(u, v), Normal(nx, ny, nz) 
	{

	}

	DirectX::XMFLOAT3 Position;	    // The position of the vertex
	DirectX::XMFLOAT2 UV;           // UV Coordinate for texturing
	DirectX::XMFLOAT3 Normal;       // Normal for lighting
	DirectX::XMFLOAT3 Tangent;		// For normal mapping
	DirectX::XMFLOAT4 Colour;
};