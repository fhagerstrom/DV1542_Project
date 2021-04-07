#pragma once

#include <d3d11.h>

#include "Vertex.h"


class Mesh
{
public:
	Mesh(Vertex* vertArray, int numVerts, unsigned int* indexArray, int numIndices, ID3D11Device* device);
	Mesh(ID3D11Device* device, const char* objFile);
	~Mesh();

	ID3D11Buffer* GetVertexBuffer() { return vBuffer; }
	ID3D11Buffer* GetIndexBuffer() { return iBuffer; }
	int GetIndexCount() { return numIndices; }

private:
	ID3D11Buffer* vBuffer;
	ID3D11Buffer* iBuffer;
	int numIndices;

	void CalculateTangents(Vertex* verts, int numVerts, unsigned int* indices, int numIndices);
	void CreateBuffers(Vertex* vertArray, int numVerts, unsigned int* indexArray, int numIndices, ID3D11Device* device);
};

