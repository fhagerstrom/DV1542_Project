#pragma once

#include <string>
#include <fstream>
#include <d3d11.h>

using namespace std;

class Converter
{
private:
	typedef struct
	{
		float x, y, z;
	}VertexType;

	typedef struct
	{
		int vIndex1, vIndex2, vIndex3;
		int tIndex1, tIndex2, tIndex3;
		int nIndex1, nIndex2, nIndex3;
	}FaceType;

public:
	Converter();
	Converter(const Converter&);
	~Converter();

	int Convert(WCHAR*, int);
	bool ReadFileCounts(WCHAR*, int&, int&, int&, int&);
	bool LoadDataStructures(WCHAR*, int, int, int, int);
};