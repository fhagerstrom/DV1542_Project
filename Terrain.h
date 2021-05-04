#pragma once
#include <d3d11.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <stdio.h>
#include <DirectXMath.h>

#include "Vertex.h"

namespace dx = DirectX;

// @TODO: 
// - Rework code to use vectors instead of arrays.
// - Use Vertex Struct instead of HeightmapType and ModelType.
// - Fix mesh for terrain so it can be drawn.

//struct HeightMapType
//{
//	float x, y, z;
//};
//
//struct ModelType
//{
//	float x, y, z;
//};

struct HeightMapInfo
{
	int terrainWidth;
	int terrainHeight;
	dx::XMFLOAT3* heightMap;
};

class Terrain
{
private:
	ID3D11Buffer* vBuffer;
	ID3D11Buffer* iBuffer;

	int numVertices, numFaces;

	// DESSA VEKTORER MÅSTE VI GÖRA TILLGÄNGLIGA I ANDRA FUNKTIONER I KLASSEN
	std::vector<Vertex> v; 
	std::vector<DWORD> indices;

	/*int vCount, iCount;
	int terrainHeight, terrainWidth;

	float heightScale;
	char* terrainFile;

	HeightMapType* heightMap;
	ModelType* terrainModel;*/

public:
	int getIndexCount;

	Terrain();
	Terrain(const Terrain& other);
	~Terrain();

	bool LoadHeightMap(const char* filename, HeightMapInfo& hmInfo);
	bool InitTerrain(HeightMapInfo& hmInfo);
	/*bool LoadSetup(const char* filename);
	bool LoadHeightMap();
	bool BuildTerrainModel();*/
	bool InitBuffers(ID3D11Device* device);

	void RenderTerrain(ID3D11DeviceContext* context);
	// void SetTerrainCoordinates();
};
