#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <fstream>

#include "Vertex.h"

class Mesh
{
private:
	int numIndices;

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	void CreateBuffers(ID3D11Device* device, Vertex* vertexArray, int numVertices, unsigned int* indexArray, int numIndices);
	// void CalculateTangents(Vertex* vertices, int numVertices, unsigned int* indices, int numIndices);

public:
	Mesh(ID3D11Device* device, Vertex* vertexArray, int numVertices, unsigned int* indexArray, int numIndices);
	Mesh(ID3D11Device* device, const char* objFile);
	~Mesh();

	ID3D11Buffer* GetVertexBuffer() { return vertexBuffer; }
	ID3D11Buffer* GetIndexBuffer() { return indexBuffer; }
	int GetIndexCount() { return numIndices; }
};