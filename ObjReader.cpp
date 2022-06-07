#include "ObjReader.h"

ObjReader::ObjReader()
{

}

ObjReader::~ObjReader()
{
}

bool ObjReader::LoadObj(ID3D11Device* device, Model* model, std::wstring modelFile, bool rightHandCoord, bool calcNormals)
{
	std::wifstream fileRead(modelFile.c_str()); // Open file
	std::wstring mtlLib; // Material library file

	// Temp vectors for storage
	std::vector<XMFLOAT3> vertexPos;
	std::vector<XMFLOAT2> vertexTexCoord;
	std::vector<XMFLOAT3> vertexNormal;
	std::vector<std::wstring> materials;

	// Temp vectors for indexes
	std::vector<int> vertexPosIndex;
	std::vector<int> vertexTexCoordIndex;
	std::vector<int> vertexNormalIndex;

	// Temp variables for storing into the vectors
	std::wstring tempMaterials;
	int tempVertexPosIndex;
	int tempVertexTexCoordIndex;
	int tempVertexNormalIndex;

	wchar_t charCheck; // Variable for reading one char at a time from file.
	std::wstring face; // String for face vertices
	int vIndex = 0; // Index
	int totalVertices = 0; // Total vertices
	int triangleCount = 0; // Total triangles
	int meshTriangles = 0;

	bool hasTexCoord = false;
	bool hasNormals = false;

	// Check if file couldn't be opened
	if (!fileRead.is_open())
	{
		MessageBox(NULL, L"Couldn't open obj file!", L"OBJ ERROR", MB_OK);
		return false;
	}

	// While file is open, check chars inside file and do respective work
	while (fileRead)
	{
		charCheck = fileRead.get();

		switch (charCheck)
		{
			// Case for vertex info
			case 'v':
			{
				charCheck = fileRead.get();
				if (charCheck == ' ') // If there's a space after, we can get the next types
				{
					float vx, vy, vz;
					fileRead >> vx >> vy >> vz;

					if (rightHandCoord)
						vertexPos.push_back(XMFLOAT3(vx, vy, vz * -1.0f)); // Flip Z-axis if model is in RH-coord

					else
						vertexPos.push_back(XMFLOAT3(vx, vy, vz));
				}

				if (charCheck == 't') // Texture coordinate (UV)
				{
					float vtu, vtv;
					fileRead >> vtu >> vtv; // Read in texcoord values

					if (rightHandCoord)
						vertexTexCoord.push_back(XMFLOAT2(vtu, 1.0f - vtv)); // Invert UV coordinates to account for RH-coord

					else
						vertexTexCoord.push_back(XMFLOAT2(vtu, vtv)); // Insert values into vector without conversion

					hasTexCoord = true; // The model does have texture coordinates.
				}

				if (charCheck == 'n')
				{
					float vnx, vny, vnz;
					fileRead >> vnx >> vny >> vnz; // Read in the normal values

					if (rightHandCoord)
						vertexNormal.push_back(XMFLOAT3(vnx, vny, vnz * -1.0f)); // Invert z coordinate to account for RH

					else
						vertexNormal.push_back(XMFLOAT3(vnx, vny, vnz));

					hasNormals = true; // The model does have normal(s)
				}

				break;
			}

			// Groups / Submeshes
			case 'g':
			{

			}

		}
	}


	return true;
}
