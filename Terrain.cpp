#include "Terrain.h"

Terrain::Terrain()
{

}

Terrain::Terrain(const Terrain& other)
{

}

Terrain::~Terrain()
{
	/*if (heightMap)
	{
		delete[] heightMap;
		heightMap = 0;
	}

	if (terrainModel)
	{
		delete[] terrainModel;
		terrainModel = 0;
	}*/
}

bool Terrain::LoadHeightMap(const char* filename, HeightMapInfo& hmInfo)
{
	FILE* filePtr;                            // Point to the current position in the file
	BITMAPFILEHEADER bitmapFileHeader;        // Structure which stores information about file
	BITMAPINFOHEADER bitmapInfoHeader;        // Structure which stores information about image
	errno_t err;

	int imageSize, index;
	unsigned char height;

	// Open the file
	err = fopen_s(&filePtr, filename, "rb");
	if (err == NULL)
	{
		std::cerr << "ERROR! File is nullptr!" << std::endl;
	}

	// Read bitmaps header
	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);

	// Read the info header
	fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);

	// Get the width and height (width and length) of the image
	hmInfo.terrainWidth = bitmapInfoHeader.biWidth;
	hmInfo.terrainHeight = bitmapInfoHeader.biHeight;

	// Size of the image in bytes. the 3 represents RBG (byte, byte, byte) for each pixel
	imageSize = hmInfo.terrainWidth * hmInfo.terrainHeight * 3;

	// Initialize the array which stores the image data
	unsigned char* bitmapImage = new unsigned char[imageSize];

	// Set the file pointer to the beginning of the image data
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// Store image data in bitmapImage
	fread(bitmapImage, 1, imageSize, filePtr);

	// Close file
	fclose(filePtr);

	// Initialize the heightMap array (stores the vertices of our terrain)
	hmInfo.heightMap = new dx::XMFLOAT3[hmInfo.terrainWidth * hmInfo.terrainHeight];

	// We use a greyscale image, so all 3 rgb values are the same, but we only need one for the height
	// So we use this counter to skip the next two components in the image data (we read R, then skip BG)
	int k = 0;

	// We divide the height by this number to "water down" the terrains height, otherwise the terrain will
	// appear to be "spikey" and not so smooth.
	float heightFactor = 10.0f;

	// Read the image data into our heightMap array
	for (int j = 0; j < hmInfo.terrainHeight; j++)
	{
		for (int i = 0; i < hmInfo.terrainWidth; i++)
		{
			height = bitmapImage[k];

			index = (hmInfo.terrainHeight * j) + i;

			hmInfo.heightMap[index].x = (float)i;
			hmInfo.heightMap[index].y = (float)height / heightFactor;
			hmInfo.heightMap[index].z = (float)j;

			k += 3;
		}
	}

	delete[] bitmapImage;
	bitmapImage = 0;

	return true;
}

bool Terrain::InitTerrain(HeightMapInfo& hmInfo)
{
	int cols = hmInfo.terrainWidth;
	int rows = hmInfo.terrainHeight;

	//Create the grid
	numVertices = rows * cols;
	numFaces = (rows - 1) * (cols - 1) * 2;

	std::vector<Vertex> v(numVertices);

	for (DWORD i = 0; i < rows; ++i)
	{
		for (DWORD j = 0; j < cols; ++j)
		{
			v[i * cols + j].Position = hmInfo.heightMap[i * cols + j];
			v[i * cols + j].Normal = dx::XMFLOAT3(0.0f, 1.0f, 0.0f);
		}
	}

	std::vector<DWORD> indices(numFaces * 3);

	int k = 0;
	int texUIndex = 0;
	int texVIndex = 0;

	for (DWORD i = 0; i < rows - 1; i++)
	{
		for (DWORD j = 0; j < cols - 1; j++)
		{
			indices[k] = i * cols + j;        // Bottom left of quad
			v[i * cols + j].UV = dx::XMFLOAT2(texUIndex + 0.0f, texVIndex + 1.0f);

			indices[k + 1] = i * cols + j + 1;        // Bottom right of quad
			v[i * cols + j + 1].UV = dx::XMFLOAT2(texUIndex + 1.0f, texVIndex + 1.0f);

			indices[k + 2] = (i + 1) * cols + j;    // Top left of quad
			v[(i + 1) * cols + j].UV = dx::XMFLOAT2(texUIndex + 0.0f, texVIndex + 0.0f);


			indices[k + 3] = (i + 1) * cols + j;    // Top left of quad
			v[(i + 1) * cols + j].UV = dx::XMFLOAT2(texUIndex + 0.0f, texVIndex + 0.0f);

			indices[k + 4] = i * cols + j + 1;        // Bottom right of quad
			v[i * cols + j + 1].UV = dx::XMFLOAT2(texUIndex + 1.0f, texVIndex + 1.0f);

			indices[k + 5] = (i + 1) * cols + j + 1;    // Top right of quad
			v[(i + 1) * cols + j + 1].UV = dx::XMFLOAT2(texUIndex + 1.0f, texVIndex + 0.0f);

			k += 6; // next quad

			texUIndex++;
		}

		texUIndex = 0;
		texVIndex++;
	}

	// @TODO: DO NORMAL AVERAGING

	return true;
}

bool Terrain::InitBuffers(ID3D11Device* device)
{
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	dx::XMFLOAT4 colour;
	colour = dx::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);

	// Set up the vertex buffer description.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * numVertices;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = &v[0];
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &vBuffer);
	if (FAILED(result))
	{
		std::cerr << "ERROR! Could not create vertex buffer for terrain!" << std::endl;
		return false;
	}

	// Set up the index buffer description.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * numFaces * 3;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = &indices[0];
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &iBuffer);
	if (FAILED(result))
	{
		std::cerr << "ERROR! Could not create index buffer for terrain!" << std::endl;
		return false;
	}

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
	context->IASetVertexBuffers(1, 1, &vBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	context->IASetIndexBuffer(iBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

//bool Terrain::LoadSetup(const char* filename)
//{
//	int strLength;
//	std::ifstream fin;
//	char input;
//
//	// Initialize the string that will hold the terrain file name.
//	strLength = 256;
//	terrainFile = new char[strLength];
//	if (!terrainFile)
//	{
//		std::cout << "ERROR! Could not intialize terrainFile string!" << std::endl;
//		return false;
//	}
//
//	// Open the setup file. If it could not open the file then exit.
//	fin.open(filename);
//	if (fin.fail())
//	{
//		std::cout << "ERROR! Could not open setup file!" << std::endl;;
//		return false;
//	}
//
//	// Read up to the terrain file name.
//	fin.get(input);
//	while (input != ':')
//	{
//		fin.get(input);
//	}
//
//	// Read in the terrain file name.
//	fin >> terrainFile;
//
//	// Read up to the value of terrain height.
//	fin.get(input);
//	while (input != ':')
//	{
//		fin.get(input);
//	}
//
//	// Read in the terrain height.
//	fin >> terrainHeight;
//
//	// Read up to the value of terrain width.
//	fin.get(input);
//	while (input != ':')
//	{
//		fin.get(input);
//	}
//
//	// Read in the terrain width.
//	fin >> terrainWidth;
//
//	// Read up to the value of terrain height scaling.
//	fin.get(input);
//	while (input != ':')
//	{
//		fin.get(input);
//	}
//
//	// Read in the terrain height scaling.
//	fin >> heightScale;
//
//	// Close the setup file.
//	fin.close();
//
//	std::cout << "Terrain Height: " << terrainHeight << "\nTerrain Width: " << terrainWidth << "\nTerrain Scale: " << heightScale << std::endl;
//
//	return true;
//}

//bool Terrain::LoadHeightMap()
//{
//	int error;
//	int imgSize;
//	int index;
//
//	FILE* filePtr;
//	unsigned long long count;
//
//	BITMAPFILEHEADER bitmapFileHeader;
//	BITMAPINFOHEADER bitmapInfoHeader;
//	unsigned char* bitmapImage;
//	unsigned char height;
//
//	// Create the array structure to hold the height map data.
//	heightMap = new HeightMapType[terrainWidth * terrainHeight];
//	if (!heightMap)
//	{
//		printf("ERROR! Could not create array structure to hold height map data!\n");
//		return false;
//	}
//
//	// Open the bitmap map file in binary.
//	error = fopen_s(&filePtr, terrainFile, "rb");
//	if (error != 0)
//	{
//		std::cout << "ERROR! Could not open bitmap map file in binary" << std::endl;
//		return false;
//	}
//	
//	// Read in the bitmap file header.
//	count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
//	if (count != 1)
//	{
//		std::cout << "ERROR! Could not read bitmap file header!" << std::endl;
//		return false;
//	}
//
//	// Read in the bitmap info header.
//	count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
//	if (count != 1)
//	{
//		std::cout << "ERROR! Could not read bitmap info header!\n" << std::endl;
//		return false;
//	}
//
//	// Make sure the height map dimensions are the same as the terrain dimensions for easy 1 to 1 mapping.
//	if ((bitmapInfoHeader.biHeight != terrainHeight) || (bitmapInfoHeader.biWidth != terrainWidth))
//	{
//		std::cout << "ERROR! Height map dimensions are not the same as terrain dimensions!" << std::endl;
//		return false;
//	}
//
//	// Calculate the size of the bitmap image data.  
//	// Since we use non-divide by 2 dimensions (eg. 257x257) we need to add an extra byte to each line.
//	imgSize = terrainHeight * terrainWidth * 3;
//
//	// Allocate memory for the bitmap image data.
//	// int row_padded = (terrainWidth * 3 + 3) & (~3);
//
//	bitmapImage = new unsigned char[imgSize];
//	if (!bitmapImage)
//	{
//		printf("Error! Could not allocate memory for bitmap image data!\n");
//		return false;
//	}
//
//	// Move to the beginning of the bitmap data.
//	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);
//
//	// Read in the bitmap image data.
//	count = fread(bitmapImage, 1, imgSize, filePtr);
//	if (count != imgSize)
//	{
//		std::cerr << "Error: The bitmap image data could not be read!" << std::endl;
//		return false;
//	}
//
//	for (size_t i = 0; i < 48; i++)
//	{
//		std::cout << bitmapImage[i] << std::endl;
//	}
//
//	// Close the file.
//	error = fclose(filePtr);
//	if (error != 0)
//	{
//		std::cout << "Couldn't close terrain file!" << std::endl;
//		return false;
//	}
//
//	// Initialize the position in the image data buffer.
//	int k = 0;
//
//	// Read the image data into the height map array.
//	for (int j = 0; j < terrainHeight; j++)
//	{
//		for (int i = 0; i < terrainWidth; i++)
//		{
//			// Bitmaps are upside down so load bottom to top into the height map array.
//			index = (terrainWidth * (terrainHeight - 1 - j)) + i;
//
//			// Get the grey scale pixel value from the bitmap image data at this location.
//			height = bitmapImage[k];
//
//			// Store the pixel value as the height at this point in the height map array.
//			heightMap[index].y = (float)height;
//			// std::cout << "Height: " << height << std::endl;
//
//			// Increment the bitmap image data index.
//			k += 3;
//		}
//
//		// Compensate for the extra byte at end of each line in non-divide by 2 bitmaps (eg. 257x257).
//		k++;
//	}
//
//	// Release the bitmap image data now that the height map array has been loaded.
//	delete[] bitmapImage;
//	bitmapImage = 0;
//
//	// Release the terrain filename now that is has been read in.
//	delete[] terrainFile;
//	terrainFile = 0;
//
//	return true;
//}

//void Terrain::SetTerrainCoordinates()
//{
//	int index;
//
//	// Loop through all the elements in the height map array and adjust their coordinates correctly.
//	for (int j = 0; j < terrainHeight; j++)
//	{
//		for (int i = 0; i < terrainWidth; i++)
//		{
//			index = (terrainWidth * j) + i;
//
//			// Set the X and Z coordinates.
//			heightMap[index].x = (float)i;
//			heightMap[index].z = -(float)j;
//
//			// Move the terrain depth into the positive range.  E.g. (0, -256) to (256, 0).
//			heightMap[index].z += (float)(terrainHeight - 1);
//
//			// Scale the height.
//			heightMap[index].y /= heightScale;
//		}
//	}
//}
//
//bool Terrain::BuildTerrainModel()
//{
//	int i, j, index, index1, index2, index3, index4;
//
//	// Calculate the number of vertices in the terrain model.
//	vCount = (terrainHeight - 1) * (terrainWidth - 1) * 6;
//
//	// Create the 3D terrain model array.
//	terrainModel = new ModelType[vCount];
//	if (!terrainModel)
//	{
//		std::cerr << "Couldn't open 3D terrain model!" << std::endl;
//		return false;
//	}
//
//	// Initialize the index into the height map array.
//	index = 0;
//
//	// Load the 3D terrain model with the height map terrain data.
//	// Create 2 triangles for each of the four points in a quad.
//	for (j = 0; j < (terrainHeight - 1); j++)
//	{
//		for (i = 0; i < (terrainWidth - 1); i++)
//		{
//			// Get the indexes to the four points of the quad.
//			index1 = (terrainWidth * j) + i;          // Upper left.
//			index2 = (terrainWidth * j) + (i + 1);      // Upper right.
//			index3 = (terrainWidth * (j + 1)) + i;      // Bottom left.
//			index4 = (terrainWidth * (j + 1)) + (i + 1);  // Bottom right.
//
//			// Now create two triangles for that quad.
//			// Triangle 1 - Upper left.
//			terrainModel[index].x = heightMap[index1].x;
//			terrainModel[index].y = heightMap[index1].y;
//			terrainModel[index].z = heightMap[index1].z;
//			index++;
//
//			// Triangle 1 - Upper right.
//			terrainModel[index].x = heightMap[index2].x;
//			terrainModel[index].y = heightMap[index2].y;
//			terrainModel[index].z = heightMap[index2].z;
//			index++;
//
//			// Triangle 1 - Bottom left.
//			terrainModel[index].x = heightMap[index3].x;
//			terrainModel[index].y = heightMap[index3].y;
//			terrainModel[index].z = heightMap[index3].z;
//			index++;
//
//			// Triangle 2 - Bottom left.
//			terrainModel[index].x = heightMap[index3].x;
//			terrainModel[index].y = heightMap[index3].y;
//			terrainModel[index].z = heightMap[index3].z;
//			index++;
//
//			// Triangle 2 - Upper right.
//			terrainModel[index].x = heightMap[index2].x;
//			terrainModel[index].y = heightMap[index2].y;
//			terrainModel[index].z = heightMap[index2].z;
//			index++;
//
//			// Triangle 2 - Bottom right.
//			terrainModel[index].x = heightMap[index4].x;
//			terrainModel[index].y = heightMap[index4].y;
//			terrainModel[index].z = heightMap[index4].z;
//			index++;
//		}
//	}
//
//	/*for (int i = 0; i < 16; i++)
//	{
//		std::cout << "Terrainmodel x: " << terrainModel[i].x << std::endl;
//		std::cout << "Terrainmodel y: " << terrainModel[i].y << std::endl;
//		std::cout << "Terrainmodel z: " << terrainModel[i].z << std::endl;
//	}*/
//
//	return true;
//}

