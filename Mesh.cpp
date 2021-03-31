#include "Mesh.h"

namespace dx = DirectX;

Mesh::Mesh(ID3D11Device* device, Vertex* vertexArray, int numVertices, unsigned int* indexArray, int numIndices)
{
	CreateBuffers(device, vertexArray, numVertices, indexArray, numIndices);
}

Mesh::Mesh(ID3D11Device* device, const char* objFile)
{
	// File object.
	std::ifstream obj(objFile);

	// Check if the file opened successfully.
	if (!obj.is_open())
	{
		// Search Debug folder.
		char debugFolder[256] = {};
		strcat_s(debugFolder, "Debug/");
		strcat_s(debugFolder, objFile);

		// Try opening again.
		obj.open(debugFolder);

		// Abort if not found.
		if (!obj.is_open())
			return;
	}

	// Variables for while reading file.
	std::vector<dx::XMFLOAT3> positions; // Positions from file.
	std::vector<dx::XMFLOAT3> normals; // normals from file.
	std::vector<dx::XMFLOAT2> texelCoords; // UVs from file.
	std::vector<Vertex> vertices; // vertices to assemble.
	std::vector<UINT> indices; // Indices of the vertices.

	unsigned int vertexCounter = 0; // Count vertices.
	char lineRead[256]; // chars for reading.

	while (obj.good())
	{
		// Get line in file.
		obj.getline(lineRead, 256);

		// Check what type of character the line starts with in the obj. file.
		// Vertices.
		if (lineRead[0] == 'v')
		{
			// Read the numbers into an XMFLOAT3
			dx::XMFLOAT3 pos;
			sscanf_s(lineRead, "v %f %f %f", &pos.x, &pos.y, &pos.z);

			positions.push_back(pos);
		}

		else if (lineRead[0] == 'v' && lineRead[1] == 't')
		{
			dx::XMFLOAT2 uv;
			sscanf_s(lineRead, "vt %f %f", &uv.x, &uv.y);

			texelCoords.push_back(uv);
		}
		
		else if (lineRead[0] == 'v' && lineRead[1] == 'n')
		{
			dx::XMFLOAT3 normal;
			sscanf_s(lineRead, "vn %f %f %f", &normal.x, &normal.y, &normal.z);

			normals.push_back(normal);
		}
		
		else if (lineRead[0] == 'f')
		{
			// Read face indices to an array
			unsigned int i[12];

			int readFaces = sscanf_s(lineRead, "f %d%d%d %d%d%d %d%d%d %d%d%d", &i[0], &i[1], &i[2], &i[3], &i[4], &i[5],
																				&i[6], &i[7], &i[8], &i[9], &i[10], &i[11]);
			// Create vertices based on corresponding data from vectors.
			// obj files are 1-based -> They need adjustment.
			Vertex v1;
			v1.position = positions[i[0] - 1];
			v1.uv = texelCoords[i[1] - 1];
			v1.normal = normals[i[2] - 1];

			Vertex v2;
			v2.position = positions[i[3] - 1];
			v2.uv = texelCoords[i[4] - 1];
			v2.normal = normals[i[5] - 1];

			Vertex v3;
			v3.position = positions[i[6] - 1];
			v3.uv = texelCoords[i[7] - 1];
			v3.normal = normals[i[8] - 1];

			// The model is made in a right-handed space. We need to convert it to left-handed for DX.
			// Following operations need to be made:
			// - Invert Z position.
			// - Invert normal's Z coordinate.
			// - Flip winding order.
			// - Also flip UV coordinates.

			// Flipping UV's.
			v1.uv.y = 1.0f - v1.uv.y;
			v2.uv.y = 1.0f - v2.uv.y;
			v3.uv.y = 1.0f - v3.uv.y;

			// Invert Z position.
			v1.position.z *= -1.0f;
			v2.position.z *= -1.0f;
			v3.position.z *= -1.0f;

			// Invert normal Z.
			v1.normal.z *= -1.0f;
			v2.normal.z *= -1.0f;
			v3.normal.z *= -1.0f;

			// Add vertices to the std::vector.
			vertices.push_back(v1);
			vertices.push_back(v2);
			vertices.push_back(v3);

			// Add three more indices.
			indices.push_back(vertexCounter); vertexCounter += 1;
			indices.push_back(vertexCounter); vertexCounter += 1;
			indices.push_back(vertexCounter); vertexCounter += 1;

			if (readFaces == 12)
			{
				// Make the last vertex.
				Vertex v4;
				v4.position = positions[i[9] - 1];
				v4.uv = texelCoords[i[10] - 1];
				v4.normal = normals[i[11] - 1];

				// Flip the UV, Z pos and normal.
				v4.uv.y = 1.0f - v4.uv.y;
				v4.position.z *= -1.0f;
				v4.normal.z *= -1.0f;

				// Add a whole triangle (flipping the winding order).
				vertices.push_back(v1);
				vertices.push_back(v4);
				vertices.push_back(v3);

				// Add three more indices
				indices.push_back(vertexCounter); vertexCounter += 1;
				indices.push_back(vertexCounter); vertexCounter += 1;
				indices.push_back(vertexCounter); vertexCounter += 1;
			}
		}
	}

	// Close the file and create the actual buffers
	obj.close();
	CreateBuffers(device, &vertices[0], vertexCounter, &indices[0], vertexCounter);
}

Mesh::~Mesh()
{
	indexBuffer->Release();
	vertexBuffer->Release();
}

void Mesh::CreateBuffers(ID3D11Device* device, Vertex* vertexArray, int numVertices, unsigned int* indexArray, int numIndices)
{
	// Create the vertex buffer.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * numVertices; // Number of vertices
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA initialVertexData;
	initialVertexData.pSysMem = vertexArray;
	device->CreateBuffer(&vertexBufferDesc, &initialVertexData, &vertexBuffer);

	// Create the index buffer.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.ByteWidth = sizeof(unsigned int) * numIndices; // Number of indices
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA initialIndexData;
	initialIndexData.pSysMem = indexArray;
	device->CreateBuffer(&indexBufferDesc, &initialIndexData, &indexBuffer);

	// Save the indices.
	this->numIndices = numIndices;
}
