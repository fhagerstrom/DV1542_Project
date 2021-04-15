#include "Terrain.h"

Terrain::Terrain()
{

}

Terrain::~Terrain()
{

}

//bool LoadHeightMap(char* filename, HeightMapInfo& hmInfo)
//{
//	FILE* stream, stream2;
//	BITMAPFILEHEADER bitmapFileHeader;
//	BITMAPINFOHEADER bitmapInfoHeader;
//
//	int imgSize;
//	int index;
//	unsigned char height;
//
//	// Open file.
//	fileptr = fopen_s(&stream, filename, "rb");
//	if (fileptr == nullptr)
//	{
//		return 0;
//	}
//
//	// Read bitmap header.
//	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, fileptr);
//
//	// Read info header.
//	fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, fileptr);
//
//	// Get width and height (length) of image.
//	hmInfo.terrainWidth = bitmapInfoHeader.biWidth;
//	hmInfo.terrainHeight = bitmapInfoHeader.biHeight;
//
//	// Size of image in bytes.
//	imgSize = hmInfo.terrainWidth * hmInfo.terrainHeight * 3;
//
//	// Initialize the array for image data.
//	unsigned char* bitmapImg = new unsigned char[imgSize];
//
//	// Set file pointer to beginning of image data.
//	fseek(fileptr, bitmapFileHeader.bfOffBits, SEEK_SET);
//
//
//}