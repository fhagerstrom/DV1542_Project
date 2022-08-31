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

						// STORING THE FACE // 
						// Check to see if there are atleast one subset
						// Subset is the same as if the model would be several meshes
						if (model->GetSubsetCount() == 0)
						{
							model->GetSubsetIndexVector().push_back(vIndex); // Start index 
							model->GetSubsetCount()++;
						}

						// Store the vertex information
						vertexPosIndex.push_back(tempVertexPosIndex);
						vertexTCIndex.push_back(tempVertexTCIndex);
						vertexNormalIndex.push_back(tempVertexNormalIndex);
						totalVertices++; // Adds a new vertex
						model->GetIndices().push_back(totalVertices - 1); // Sets the index for this vertex

						// If this is the first vertex in the face, we need to make sure the rest of the triangles use this 
						if (i == 0)
						{
							firstIndex = model->GetIndices()[vIndex]; // First index of this face
						}

						// If this was the last vertex in the first triangle, we will make sure the next triangle uses this
						if (i == 2)
						{
							lastIndex = model->GetIndices()[vIndex]; // Last index of this triangle
						}
						vIndex++; // Increase index
					}
					meshTriangles++; // Increase triangles

				}
			}
			break;

			// MATERIAL CASE
		case 'm': // mtllib
			charCheck = fileRead.get();
			if (charCheck == 't')
			{
				charCheck = fileRead.get();
				if (charCheck == 'l')
				{
					charCheck = fileRead.get();
					if (charCheck == 'l')
					{
						charCheck = fileRead.get();
						if (charCheck == 'i')
						{
							charCheck = fileRead.get();
							if (charCheck == 'b')
							{
								charCheck = fileRead.get();
								if (charCheck == ' ')
								{
									fileRead >> mtlLib;
								}
							}
						}
					}
				}
			}
			break;

			// USEMTL
		case 'u': // usemtl
			charCheck = fileRead.get();
			if (charCheck == 's')
			{
				charCheck = fileRead.get();
				if (charCheck == 'e')
				{
					charCheck = fileRead.get();
					if (charCheck == 'm')
					{
						charCheck = fileRead.get();
						if (charCheck == 't')
						{
							charCheck = fileRead.get();
							if (charCheck == 'l')
							{
								charCheck = fileRead.get();
								if (charCheck == ' ')
								{
									tempMaterials = L"";
									fileRead >> tempMaterials;
									materials.push_back(tempMaterials);
								}
							}
						}
					}
				}
			}
			break;
		default:
			break;
		}
		}
	}

	// Push back vertices into models subset index
	model->GetSubsetIndexVector().push_back(vIndex);

	// Make sure that the first subset doesn't contain 0 indices
	// Has to do with obj files defining groups before faces at times
	if (model->GetSubsetIndexVector()[1] == 0)
	{
		model->GetSubsetIndexVector().erase(model->GetSubsetIndexVector().begin() + 1);
		model->GetSubsetCount()--;
	}

	// Default values for normals and texture coords if they're not specified
	if (!hasNormals)
	{
		vertexNormal.push_back(XMFLOAT3(0.0f, 0.0f, 0.0f));
	}

	if (!hasTexCoord)
	{
		vertexTexCoord.push_back(XMFLOAT2(0.0f, 0.0f));
	}

	// Close the obj file
	fileRead.close();

	// Open mtl file
	fileRead.open(mtlLib.c_str());

	std::wstring lastStringRead;
	int materialCount = (int)model->GetMaterials().size(); // Amount of materials

	bool setDiffuse = false; // Will be used to see if the material has diffuse or not

	if (fileRead)
	{
		while (fileRead)
		{
			charCheck = fileRead.get();

			switch (charCheck)
			{
				// Check for comments
				case '#':
					charCheck = fileRead.get();
					while (charCheck != '\n')
					{
						charCheck = fileRead.get();
					}

					break;

					// Diffuse
				case 'K':
					charCheck = fileRead.get();
					if (charCheck == 'd')
					{
						charCheck = fileRead.get(); // Remove space
						fileRead >> model->GetMaterials()[materialCount - (long long)1].diffuseMtl.x;
						fileRead >> model->GetMaterials()[materialCount - (long long)1].diffuseMtl.y;
						fileRead >> model->GetMaterials()[materialCount - (long long)1].diffuseMtl.z;

						setDiffuse = true;
					}

					// Set the ambient color as diffuse if there was no diffuse
					if (charCheck == 'a')
					{
						charCheck = fileRead.get();
						if (setDiffuse == false)
						{
							fileRead >> model->GetMaterials()[materialCount - (long long)1].diffuseMtl.x;
							fileRead >> model->GetMaterials()[materialCount - (long long)1].diffuseMtl.y;
							fileRead >> model->GetMaterials()[materialCount - (long long)1].diffuseMtl.z;
						}
					}

					else
					{
						fileRead >> model->GetMaterials()[materialCount - (long long)1].ambientMtl.x;
						fileRead >> model->GetMaterials()[materialCount - (long long)1].ambientMtl.y;
						fileRead >> model->GetMaterials()[materialCount - (long long)1].ambientMtl.z;
					}

					// Specular
					if (charCheck == 's')
					{
						charCheck = fileRead.get();
						fileRead >> model->GetMaterials()[materialCount - (long long)1].specularMtl.x;
						fileRead >> model->GetMaterials()[materialCount - (long long)1].specularMtl.y;
						fileRead >> model->GetMaterials()[materialCount - (long long)1].specularMtl.z;
					}
					break;
 
					// CHECK FOR TRANSPARENCY
					charCheck = fileRead.get();
					if (charCheck == 'r')
					{
						charCheck = fileRead.get();
						float transparency;
						fileRead >> transparency;

						model->GetMaterials()[materialCount - (long long)1].isTransparent = true;
					}
					break;

					// Specular shine
				case 'N':
					charCheck = fileRead.get();
					if (charCheck == 's')
					{
						charCheck = fileRead.get();
						fileRead >> model->GetMaterials()[0].shine;
						model->GetMaterials()[0].specularMtl.w = model->GetMaterials()[0].shine;
					}
					break;

					// Normal / bump mapping
				case 'b':
					charCheck = fileRead.get();
					if (charCheck == 'u')
					{
						charCheck = fileRead.get();
						if (charCheck == 'm')
						{
							charCheck = fileRead.get();
							if (charCheck == 'p')
							{
								fileRead.get();
								fileRead.get();
								charCheck = fileRead.get();
								if (charCheck == '-')
								{
									charCheck = fileRead.get();
									if (charCheck == 'b')
									{
										charCheck = fileRead.get();
										if (charCheck == 'm')
										{
											fileRead.get();
											fileRead.get();
											fileRead.get();

											std::wstring fileName;
											bool ended = false;
											while (!ended)
											{
												// Store the filename
												charCheck = fileRead.get();
												fileName += charCheck;
												if (charCheck == '.')
												{
													for (int i = 0; i < 3; i++)
													{
														fileName += fileRead.get();

													}
													ended = true;
												}
											}

											// Create the normal map and store it to the model
											ID3D11ShaderResourceView* tempNormal;
											model->GetTextureNameVector().push_back(fileName.c_str());
											model->LoadNormalMap(device, fileName.c_str());
											model->GetMaterials()[0].hasNormalMap = true;
										}
									}
								}
							}
						}
					}
					break;

					// Texture map
				case 'm':
					charCheck = fileRead.get();
					if (charCheck == 'a')
					{
						charCheck = fileRead.get();
						if (charCheck == 'p')
						{
							charCheck = fileRead.get();
							if (charCheck == '_')
							{
								// Diffuse map = map_kd
								charCheck = fileRead.get();
								if (charCheck == 'K')
								{
									charCheck = fileRead.get();
									if (charCheck == 'd')
									{
										std::wstring filePath;
										fileRead.get(); // Removes the whitespace between map_kd and file

										// Get the file path by reading char by char
										bool filePathEnded = false;
										while (filePathEnded == false)
										{
											charCheck = fileRead.get();
											filePath += charCheck;
											if (charCheck == '.')
											{
												for (int i = 0; i < 3; i++)
												{
													filePath += fileRead.get();
												}
												filePathEnded = true;
											}
										}
										// Check if this texture has already been loaded
										bool alreadyLoaded = false;
										for (int i = 0; (unsigned)i < model->GetTextureNameVector().size(); i++)
										{
											MessageBox(0, L"Error", L"Error", MB_OK);
											if (filePath == model->GetTextureNameVector()[i])
											{
												alreadyLoaded = true;
												model->GetMaterials()[materialCount - (long long)1].textureArrayIndex = i;
												model->GetMaterials()[materialCount - (long long)1].hasTexture = true;
											}
										}

										// If the file hasnt been loaded before we can load it now
										if (alreadyLoaded == false)
										{
											ID3D11ShaderResourceView* tempMeshSRV;
											this->hr = DirectX::CreateWICTextureFromFile(device, filePath.c_str(), nullptr, &tempMeshSRV);
											assert(SUCCEEDED(hr));
											if (SUCCEEDED(this->hr))
											{
												model->GetTextureNameVector().push_back(filePath.c_str());
												model->GetMaterials()[materialCount - (long long)1].textureArrayIndex = 1; // TESTA DETTA
												//model->SetTexture(tempMeshSRV);// meshShaderResourceView.push_back(tempMeshSRV);
												model->LoadTextureObj(tempMeshSRV);
												model->GetMaterials()[materialCount - (long long)1].hasTexture = true;
											}
										}
									}
								}

								// alpha map - map_d
								else if (charCheck == 'd')
								{
									model->GetMaterials()[materialCount - (long long)1].isTransparent = true;
								}
							}
						}
					}
					break;

					// newmtl - Declares a new material
				case 'n':
					charCheck = fileRead.get();
					if (charCheck == 'e')
					{
						charCheck = fileRead.get();
						if (charCheck == 'w')
						{
							charCheck = fileRead.get();
							if (charCheck == 'm')
							{
								charCheck = fileRead.get();
								if (charCheck == 't')
								{
									charCheck = fileRead.get();
									if (charCheck == 'l')
									{
										charCheck = fileRead.get();
										if (charCheck == ' ')
										{
											// New material, set default values
											// Pushes it back to the model
											// This will be done first when entering the MTL file
											Material tempMat;
											model->GetMaterials().push_back(tempMat);
											fileRead >> model->GetMaterials()[materialCount].mtlName;
											model->GetMaterials()[materialCount].isTransparent = false;
											model->GetMaterials()[materialCount].hasTexture = false;
											model->GetMaterials()[materialCount].textureArrayIndex = 0;
											model->GetMaterials()[materialCount].translation = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
											materialCount++;
											setDiffuse = false;

										}
									}
								}
							}
						}
					}
					break;
				default:
					break;
			}
		}
	}

	// Set the subsets material to the index value of its material in the material array
	// Subsets will only be used if a model contains of more than 1 mesh
	bool hasMtl = false;
	for (int i = 0; i < model->GetSubsetCount(); i++)
	{
		for (int j = 0; (unsigned)j < model->GetMaterials().size(); j++)
		{
			if (materials[i] == model->GetMaterials()[j].mtlName)
			{
				model->GetSubsetMaterialVector().push_back(j);
				hasMtl = true;
			}
		}
		if (hasMtl == false)
		{
			model->GetSubsetMaterialVector().push_back(0); // Use the first material
		}
	}

	Vertex tempVertex;

	// Create the vertices
	for (int j = 0; j < totalVertices; j++)
	{
		tempVertex.position = vertexPos[vertexPosIndex[j]];
		tempVertex.texCoord = vertexTexCoord[vertexTCIndex[j]];
		tempVertex.normal = vertexNormal[vertexNormalIndex[j]];

		model->GetVertices().push_back(tempVertex);
	}

	// TODO:
	// CALCULATE NORMALS



	model->SetVertexCount((int)model->GetVertices().size());
	model->SetIndexCount((int)model->GetIndices().size());

	// Create index buffer
	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * meshTriangles * 3;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	ID3D11Buffer* indexBuffer;
	ID3D11Buffer* vertexBuffer;

	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &model->GetIndices()[0];
	this->hr = device->CreateBuffer(&indexBufferDesc, &iinitData, &indexBuffer);
	assert(SUCCEEDED(this->hr));

	// Create vertex buffer
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * totalVertices;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;
	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = &model->GetVertices()[0];
	this->hr = device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &vertexBuffer);
	assert(SUCCEEDED(this->hr));

	model->SetIndexBuffer(indexBuffer);
	model->SetVertexBuffer(vertexBuffer);

	return true;
}
