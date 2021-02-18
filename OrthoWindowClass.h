#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <iostream>

namespace dx = DirectX;

class OrthoWindowClass
{
private:

	struct VertexType
	{
		dx::XMFLOAT3 position;
		dx::XMFLOAT2 texture;
	};

public:
	OrthoWindowClass();
	OrthoWindowClass(const OrthoWindowClass&);
	~OrthoWindowClass();

	bool Initialize(ID3D11Device*, int, int);
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();

private:
	bool InitializeBuffers(ID3D11Device*, int, int);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

private:

	ID3D11Buffer *vertexBuffer, *indexBuffer;
	int vertexCount, indexCount;
};