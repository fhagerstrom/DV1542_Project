#include "Mesh.h"

using namespace DirectX;

Mesh::Mesh(ID3D11Device* device, Vertex* verticeArray, int numVertices, unsigned int* indexArray, int numIndices)
{
	CreateBuffers(device, verticeArray, numVertices, indexArray, numIndices);
}

Mesh::Mesh(ID3D11Device* device, const char* objFile)
{
	// File input object
	std::ifstream obj(objFile);

	// Check for successful open
	if (!obj.is_open())
	{
		// Check the debug folder
		char debugFolder[256] = {};
		strcat_s(debugFolder, "Debug/");
		strcat_s(debugFolder, objFile);

		// Attempt to open again
		obj.open(debugFolder);

		// If not found, give up
		if (!obj.is_open())
		{
			std::cout << "Obj file not found!" << std::endl;
		}
	}

	// Variables used while reading the file
	std::vector<XMFLOAT3> positions;     // Positions from the file
	std::vector<XMFLOAT3> normals;       // Normals from the file
	std::vector<XMFLOAT2> uvs;           // UVs from the file
	std::vector<Vertex> vertices;        // Vertices we're assembling
	std::vector<UINT> indices;           // Indices of these verts
	unsigned int verticeCounter = 0;     // Count of vertices/indices
	char lineRead[255];                  // String for line reading

	while (obj.good())
	{
		// Get the line
		obj.getline(lineRead, 255);

		// Check the type of line
		if (lineRead[0] == 'v' && lineRead[1] == 'n')
		{
			// Read the 3 numbers directly into an XMFLOAT3
			XMFLOAT3 norm;
			sscanf_s(lineRead, "vn %f %f %f", &norm.x, &norm.y, &norm.z);

			// Add to the list of normals
			normals.push_back(norm);
		}
		else if (lineRead[0] == 'v' && lineRead[1] == 't')
		{
			// Read the 2 numbers directly into an XMFLOAT2
			XMFLOAT2 uv;
			sscanf_s(lineRead, "vt %f %f", &uv.x, &uv.y);

			// Add to the list of uv's
			uvs.push_back(uv);
		}
		else if (lineRead[0] == 'v')
		{
			// Read the 3 numbers directly into an XMFLOAT3
			XMFLOAT3 pos;
			sscanf_s(lineRead, "v %f %f %f", &pos.x, &pos.y, &pos.z);

			// Add to the positions
			positions.push_back(pos);
		}
		else if (lineRead[0] == 'f')
		{
			// Read the face indices into an array
			unsigned int i[12];
			int facesRead = sscanf_s(lineRead,
				"f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", &i[0], &i[1], &i[2], &i[3], &i[4], &i[5],
														 &i[6], &i[7], &i[8], &i[9], &i[10], &i[11]);

			// - Create the vertices by looking up corresponding data from vectors
			// - OBJ File indices are 1-based opposed to DX, adjust them
			Vertex v1;
			v1.position = positions[i[0] - 1];
			v1.uv = uvs[i[1] - 1];
			v1.normal = normals[i[2] - 1];

			Vertex v2;
			v2.position = positions[i[3] - 1];
			v2.uv = uvs[i[4] - 1];
			v2.normal = normals[i[5] - 1];

			Vertex v3;
			v3.position = positions[i[6] - 1];
			v3.uv = uvs[i[7] - 1];
			v3.normal = normals[i[8] - 1];

			// The model is most likely in a right-handed space (Most 3D model programs uses a right-handed coordinate system).
			// We want to convert to a left-handed space for DirectX. 
			// We need to do the following operations:
			//  - Invert Z position
			//  - Invert the normal's Z
			//  - Flip the winding order
			// We also need to flip the uv coordinate 
			// since DirectX defines the origin as the top left of the texture, 
			// and many 3D modeling packages use the bottom left as the origin.

			// Flip the uv's
			v1.uv.y = 1.0f - v1.uv.y;
			v2.uv.y = 1.0f - v2.uv.y;
			v3.uv.y = 1.0f - v3.uv.y;

			// Flip Z coordinates
			v1.position.z *= -1.0f;
			v2.position.z *= -1.0f;
			v3.position.z *= -1.0f;

			// Flip normal Z
			v1.normal.z *= -1.0f;
			v2.normal.z *= -1.0f;
			v3.normal.z *= -1.0f;

			// Add the vertices to the vector (flipping the winding order)
			vertices.push_back(v1);
			vertices.push_back(v3);
			vertices.push_back(v2);

			// Add three more indices
			indices.push_back(verticeCounter); verticeCounter += 1;
			indices.push_back(verticeCounter); verticeCounter += 1;
			indices.push_back(verticeCounter); verticeCounter += 1;

			// If there is a fourth face...
			if (facesRead == 12)
			{
				// Make the last vertex
				Vertex v4;
				v4.position = positions[i[9] - 1];
				v4.uv = uvs[i[10] - 1];
				v4.normal = normals[i[11] - 1];

				// Flip the uv, Z coord and normal
				v4.uv.y = 1.0f - v4.uv.y;
				v4.position.z *= -1.0f;
				v4.normal.z *= -1.0f;

				// Add a whole triangle (flipping the winding order)
				vertices.push_back(v1);
				vertices.push_back(v4);
				vertices.push_back(v3);

				// Add three more indices
				indices.push_back(verticeCounter); verticeCounter += 1;
				indices.push_back(verticeCounter); verticeCounter += 1;
				indices.push_back(verticeCounter); verticeCounter += 1;
			}
		}
	}

	// Close the file and create the actual buffers
	obj.close();
	CreateBuffers(device, &vertices[0], verticeCounter, &indices[0], verticeCounter);
}

Mesh::~Mesh()
{
	vBuffer->Release(); 
	vBuffer = 0;

	iBuffer->Release();
	iBuffer = 0;
}

void Mesh::CreateBuffers(ID3D11Device* device, Vertex* verticeArray, int numVertices, unsigned int* indexArray, int numIndices)
{
	// Calculate the tangents before copying to buffer
	CalculateTangents(verticeArray, numVertices, indexArray, numIndices);

	// Create the vertex buffer
	D3D11_BUFFER_DESC vBufferDesc;
	vBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vBufferDesc.ByteWidth = sizeof(Vertex) * numVertices; // Number of vertices
	vBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vBufferDesc.CPUAccessFlags = 0;
	vBufferDesc.MiscFlags = 0;
	vBufferDesc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA initialVertexData;
	initialVertexData.pSysMem = verticeArray;
	device->CreateBuffer(&vBufferDesc, &initialVertexData, &vBuffer);

	// Create the index buffer
	D3D11_BUFFER_DESC iBufferDesc;
	iBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	iBufferDesc.ByteWidth = sizeof(unsigned int) * numIndices; // Number of indices
	iBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	iBufferDesc.CPUAccessFlags = 0;
	iBufferDesc.MiscFlags = 0;
	iBufferDesc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA initialIndexData;
	initialIndexData.pSysMem = indexArray;
	device->CreateBuffer(&iBufferDesc, &initialIndexData, &iBuffer);

	// Save the indices
	this->numIndices = numIndices;
}

// Calculates the tangents of the vertices in a mesh
void Mesh::CalculateTangents(Vertex* vertices, int numVertices, unsigned int* indices, int numIndices)
{
	// Reset tangents
	for (int i = 0; i < numVertices; i++)
	{
		vertices[i].tangent = XMFLOAT3(0, 0, 0);
	}

	// Calculate tangents one whole triangle at a time
	for (int i = 0; i < numVertices;)
	{
		// Grab indices and vertices of first triangle
		unsigned int i1 = indices[i++];
		unsigned int i2 = indices[i++];
		unsigned int i3 = indices[i++];
		Vertex* v1 = &vertices[i1];
		Vertex* v2 = &vertices[i2];
		Vertex* v3 = &vertices[i3];

		// Calculate vectors relative to triangle positions
		float x1 = v2->position.x - v1->position.x;
		float y1 = v2->position.y - v1->position.y;
		float z1 = v2->position.z - v1->position.z;

		float x2 = v3->position.x - v1->position.x;
		float y2 = v3->position.y - v1->position.y;
		float z2 = v3->position.z - v1->position.z;

		// Do the same for vectors relative to triangle uv's
		float s1 = v2->uv.x - v1->uv.x;
		float t1 = v2->uv.y - v1->uv.y;

		float s2 = v3->uv.x - v1->uv.x;
		float t2 = v3->uv.y - v1->uv.y;

		// Create vectors for tangent calculation
		float r = 1.0f / (s1 * t2 - s2 * t1);
		
		float tx = (t2 * x1 - t1 * x2) * r;
		float ty = (t2 * y1 - t1 * y2) * r;
		float tz = (t2 * z1 - t1 * z2) * r;

		// Adjust tangents of each vert of the triangle
		v1->tangent.x += tx; 
		v1->tangent.y += ty; 
		v1->tangent.z += tz;

		v2->tangent.x += tx; 
		v2->tangent.y += ty; 
		v2->tangent.z += tz;

		v3->tangent.x += tx; 
		v3->tangent.y += ty; 
		v3->tangent.z += tz;
	}

	// Ensure all of the tangents are orthogonal to the normals
	for (int i = 0; i < numVertices; i++)
	{
		// Grab the two vectors
		XMVECTOR normal = XMLoadFloat3(&vertices[i].normal);
		XMVECTOR tangent = XMLoadFloat3(&vertices[i].tangent);

		// Use Gram-Schmidt orthogonalization algorithm
		tangent = XMVector3Normalize(tangent - normal * XMVector3Dot(normal, tangent));
		
		// Store the tangent
		XMStoreFloat3(&vertices[i].tangent, tangent);
	}
}