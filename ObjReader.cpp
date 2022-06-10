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
	std::vector<int> vertexTCIndex;
	std::vector<int> vertexNormalIndex;

	// Temp variables for storing into the vectors
	std::wstring tempMaterials;
	int tempVertexPosIndex;
	int tempVertexTCIndex;
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
				// Push back subset index into a vector with the vIndex
				// Increment subset count
			}

			case 'f':
			{
				charCheck = fileRead.get();
				if (charCheck == ' ')
				{
					face = L"";
					std::wstring vertexDef; // hold one vertex definition at the time
					triangleCount = 0;
					charCheck = fileRead.get();

					while (charCheck != '\n')
					{
						face += charCheck; // Read next char for the face string
						charCheck = fileRead.get(); // Continue reading

						if (charCheck == ' ')
						{
							triangleCount++; // Increment triangle count
						}
					}

					// If there is a space in the end of the string, decrement the triangle count.
					// Why? Every character adds to the triangle count, so we compensate by subtracting for every space character
					if (face[face.length() - 1 == ' '])
					{
						triangleCount--;
					}
					triangleCount -= 1; // The third vertex in each face makes a new face

					std::wstringstream strStream(face); // Create a stringstream for loading face data

					if (face.length() > 0)
					{
						int firstIndex, lastIndex; // First and last vertex indices
						for (unsigned int i = 0; i < 3; i++)
						{
							strStream >> vertexDef; // Get vertex info
							std::wstring vertexPart; // (v / vt / vn)
							int partCheck = 0;

							// Loop through string
							for (unsigned int j = 0; j < vertexDef.length(); j++)
							{
								// Add a char if there is no divider
								if (vertexDef[j] != '/')
									vertexPart += vertexDef[j];

								// If current char is a divider or last char of line,
								if (vertexDef[j] == '/' || j == vertexDef.length() - 1)
								{
									// Convert strings to ints
									std::wistringstream wStoi(vertexPart);

									if (partCheck == 0) // Simply v
									{
										wStoi >> tempVertexPosIndex;
										tempVertexPosIndex -= 1; // OBJ arrays start at 1, not 0. Therefore, subtract 1

										if (j == vertexDef.length() - 1) // If only positions are used in the file
										{
											tempVertexNormalIndex = 0;
											tempVertexTCIndex = 0;
										}
									}

									else if (partCheck == 1) // vt
									{
										if (vertexPart != L"")
										{
											wStoi >> tempVertexTCIndex;
											tempVertexTCIndex -= 1;
										}

										// No texcoords, default
										else
											tempVertexTCIndex = 0;

										if (j == vertexDef.length() - 1) // If the current char is in the end of the string, there are no normals
										{
											tempVertexNormalIndex = 0; // Sets default normal value
										}
									}

									else if (partCheck == 2) // vn
									{
										std::wistringstream wStoi(vertexPart);
										wStoi >> tempVertexNormalIndex;
										tempVertexNormalIndex -= 1; // Subtract 1
									}
									vertexPart = L""; // Clear string and go to next part
									partCheck++; // Go on to next vertex part

								}
							}
							// TODO: WRITE CODE FOR STORING FACES HERE!!!

							// MODEL SUBSET

						}

					}

				}

			}

		}
	}


	return true;
}
