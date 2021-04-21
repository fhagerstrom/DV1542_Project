#include "Terrain.h"
#include <iostream>

Terrain::Terrain()
{

}

Terrain::Terrain(const Terrain& other)
{

}

Terrain::~Terrain()
{
	if (heightMap)
	{
		delete[] heightMap;
		heightMap = 0;
	}

	if (terrainModel)
	{
		delete[] terrainModel;
		terrainModel = 0;
	}
}

bool Terrain::LoadSetup(const char* filename)
{
	int strLength;
	std::ifstream fin;
	char input;

	// Initialize the string that will hold the terrain file name.
	strLength = 256;
	terrainFile = new char[strLength];
	if (!terrainFile)
	{
		printf("ERROR! Could not intialize terrainFile string!\n");
		return false;
	}

	// Open the setup file. If it could not open the file then exit.
	fin.open(filename);
	if (fin.fail())
	{
		printf("ERROR! Could not open setup file!\n");
		return false;
	}

	// Read up to the terrain file name.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// Read in the terrain file name.
	fin >> terrainFile;

	// Read up to the value of terrain height.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// Read in the terrain height.
	fin >> terrainHeight;

	// Read up to the value of terrain width.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// Read in the terrain width.
	fin >> terrainWidth;

	// Read up to the value of terrain height scaling.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// Read in the terrain height scaling.
	fin >> heightScale;

	// Close the setup file.
	fin.close();

	std::cout << "\nTerrain Height: " << terrainHeight << "\nTerrain Width: " << terrainWidth << "\nTerrain Scale: " << heightScale << std::endl;

	return true;
}

bool Terrain::LoadHeightMap()
{
	int error;
	int imgSize;
	int index;

	FILE* filePtr;
	unsigned long long count;

	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;
	unsigned char* bitmapImage;
	unsigned char height;

	// Create the array structure to hold the height map data.
	heightMap = new HeightMapType[terrainWidth * terrainHeight];
	if (!heightMap)
	{
		printf("ERROR! Could not create array structure to hold height map data!\n");
		return false;
	}

	// Open the bitmap map file in binary.
	error = fopen_s(&filePtr, terrainFile, "rb");
	if (error != 0)
	{
		printf("ERROR! Could not open bitmap map file in binary\n");
		return false;
	}
	
	// Read in the bitmap file header.
	count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	if (count != 1)
	{
		printf("ERROR! Could not read bitmap file header!\n");
		return false;
	}

	// Read in the bitmap info header.
	count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	if (count != 1)
	{
		printf("ERROR! Could not read bitmap info header!\n");
		return false;
	}

	// Make sure the height map dimensions are the same as the terrain dimensions for easy 1 to 1 mapping.
	if ((bitmapInfoHeader.biHeight != terrainHeight) || (bitmapInfoHeader.biWidth != terrainWidth))
	{
		printf("ERROR! Height map dimensions are not the same as terrain dimensions!\n");
		return false;
	}

	// Calculate the size of the bitmap image data.  
	// Since we use non-divide by 2 dimensions (eg. 257x257) we need to add an extra byte to each line.
	imgSize = terrainHeight * terrainWidth * 3;

	// Allocate memory for the bitmap image data.
	int row_padded = (terrainWidth * 3 + 3) & (~3);

	bitmapImage = new unsigned char[imgSize];
	if (!bitmapImage)
	{
		printf("Error! Could not allocate memory for bitmap image data!\n");
		return false;
	}

	// Move to the beginning of the bitmap data.
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// Read in the bitmap image data.
	count = fread(bitmapImage, 1, imgSize, filePtr);
	if (count != imgSize)
	{
		std::cerr << "Error: The bitmap image data could not be read!" << std::endl;
		return false;
	}

	for (size_t i = 0; i < 48; i++)
	{
		std::cout << bitmapImage[i] << std::endl;
	}

	// Close the file.
	error = fclose(filePtr);
	if (error != 0)
	{
		std::cout << "Couldn't close terrain file!" << std::endl;
		return false;
	}

	// Initialize the position in the image data buffer.
	int k = 0;

	// Read the image data into the height map array.
	for (int j = 0; j < terrainHeight; j++)
	{
		for (int i = 0; i < terrainWidth; i++)
		{
			// Bitmaps are upside down so load bottom to top into the height map array.
			index = (terrainWidth * (terrainHeight - 1 - j)) + i;

			// Get the grey scale pixel value from the bitmap image data at this location.
			height = bitmapImage[k];

			// Store the pixel value as the height at this point in the height map array.
			heightMap[index].y = (float)height;
			// std::cout << "Height: " << height << std::endl;

			// Increment the bitmap image data index.
			k += 3;
		}

		// Compensate for the extra byte at end of each line in non-divide by 2 bitmaps (eg. 257x257).
		k++;
	}

	// Release the bitmap image data now that the height map array has been loaded.
	delete[] bitmapImage;
	bitmapImage = 0;

	// Release the terrain filename now that is has been read in.
	delete[] terrainFile;
	terrainFile = 0;

	return true;
}

void Terrain::SetTerrainCoordinates()
{
	int index;

	// Loop through all the elements in the height map array and adjust their coordinates correctly.
	for (int j = 0; j < terrainHeight; j++)
	{
		for (int i = 0; i < terrainWidth; i++)
		{
			index = (terrainWidth * j) + i;

			// Set the X and Z coordinates.
			heightMap[index].x = (float)i;
			heightMap[index].z = -(float)j;

			// Move the terrain depth into the positive range.  E.g. (0, -256) to (256, 0).
			heightMap[index].z += (float)(terrainHeight - 1);

			// Scale the height.
			// heightMap[index].y = heightScale;
		}
	}
}

bool Terrain::BuildTerrainModel()
{
	int i, j, index, index1, index2, index3, index4;

	// Calculate the number of vertices in the terrain model.
	vCount = (terrainHeight - 1) * (terrainWidth - 1) * 6;

	// Create the 3D terrain model array.
	terrainModel = new ModelType[vCount];
	if (!terrainModel)
	{
		std::cerr << "Couldn't open 3D terrain model!" << std::endl;
		return false;
	}

	// Initialize the index into the height map array.
	index = 0;

	// Load the 3D terrain model with the height map terrain data.
	// Create 2 triangles for each of the four points in a quad.
	for (j = 0; j < (terrainHeight - 1); j++)
	{
		for (i = 0; i < (terrainWidth - 1); i++)
		{
			// Get the indexes to the four points of the quad.
			index1 = (terrainWidth * j) + i;          // Upper left.
			index2 = (terrainWidth * j) + (i + 1);      // Upper right.
			index3 = (terrainWidth * (j + 1)) + i;      // Bottom left.
			index4 = (terrainWidth * (j + 1)) + (i + 1);  // Bottom right.

			// Now create two triangles for that quad.
			// Triangle 1 - Upper left.
			terrainModel[index].x = heightMap[index1].x;
			terrainModel[index].y = heightMap[index1].y;
			terrainModel[index].z = heightMap[index1].z;
			index++;

			// Triangle 1 - Upper right.
			terrainModel[index].x = heightMap[index2].x;
			terrainModel[index].y = heightMap[index2].y;
			terrainModel[index].z = heightMap[index2].z;
			index++;

			// Triangle 1 - Bottom left.
			terrainModel[index].x = heightMap[index3].x;
			terrainModel[index].y = heightMap[index3].y;
			terrainModel[index].z = heightMap[index3].z;
			index++;

			// Triangle 2 - Bottom left.
			terrainModel[index].x = heightMap[index3].x;
			terrainModel[index].y = heightMap[index3].y;
			terrainModel[index].z = heightMap[index3].z;
			index++;

			// Triangle 2 - Upper right.
			terrainModel[index].x = heightMap[index2].x;
			terrainModel[index].y = heightMap[index2].y;
			terrainModel[index].z = heightMap[index2].z;
			index++;

			// Triangle 2 - Bottom right.
			terrainModel[index].x = heightMap[index4].x;
			terrainModel[index].y = heightMap[index4].y;
			terrainModel[index].z = heightMap[index4].z;
			index++;
		}
	}

	/*for (int i = 0; i < 16; i++)
	{
		std::cout << "Terrainmodel x: " << terrainModel[i].x << std::endl;
		std::cout << "Terrainmodel y: " << terrainModel[i].y << std::endl;
		std::cout << "Terrainmodel z: " << terrainModel[i].z << std::endl;
	}*/

	return true;
}

bool Terrain::InitBuffers(ID3D11Device* device)
{
	Vertex* vertices;
	unsigned long* indices;

	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	dx::XMFLOAT4 colour;

	colour = dx::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);

	// Calculate the number of vertices in the terrain.
	vCount = (terrainWidth - 1) * (terrainHeight - 1) * 6;

	// Set the index count to the same as the vertex count.
	iCount = vCount;

	// Create the vertex array.
	vertices = new Vertex[vCount];
	if (!vertices)
	{
		std::cerr << "Error: Couldnt create vertex array!" << std::endl;
		return false;
	}

	// Create the index array.
	indices = new unsigned long[iCount];
	if (!indices)
	{
		std::cerr << "Error: Couldnt create index array!" << std::endl;
		return false;
	}

	// Load the vertex array and index array with 3D terrain model data.
	for (int i = 0; i < vCount; i++)
	{
		vertices[i].Position = dx::XMFLOAT3(terrainModel[i].x, terrainModel[i].y, terrainModel[i].z);
		vertices[i].Colour = colour;
		indices[i] = i;
	}

	// Set up the vertex buffer description.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex)* vCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &vBuffer);
	if (FAILED(result))
	{
		printf("ERROR! Could not create vertex buffer!\n");
		return false;
	}

	// Set up the index buffer description.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * iCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &iBuffer);
	if (FAILED(result))
	{
		printf("ERROR! Could not create index buffer!\n");
		return false;
	}

	// Release the arrays now that the buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}

void Terrain::RenderTerrain(ID3D11DeviceContext* context)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(Vertex);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	context->IASetVertexBuffers(0, 1, &vBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	context->IASetIndexBuffer(iBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

