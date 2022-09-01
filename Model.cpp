#include "Model.h"


Model::Model()
{
	vertexBuffer = 0;
	indexBuffer = 0;
	texture = 0;
}

Model::Model(std::string name)
{
}

Model::~Model()
{
}

bool Model::InitializeCube(ID3D11Device* device, ID3D11DeviceContext* context, LPCWSTR textureFile)
{
	bool result;

	result = CreateCube(device);
	if (!result)
		return false;

	result = LoadTexture(device, context, textureFile);
	if (!result)
		return false;

	return true;
}

bool Model::InitializeQuad(ID3D11Device* device, ID3D11DeviceContext* context, LPCWSTR textureFile)
{
	bool result;

	result = CreateQuad(device);
	if (!result)
		return false;

	result = LoadTexture(device, context, textureFile);
	if (!result)
		return false;

	return true;
}

bool Model::LoadNormalMap(ID3D11Device* device, LPCWSTR textureFile)
{
	bool result;

	normalMap = new Texture;
	if (!normalMap)
		return false;

	result = normalMap->Initialize(device, textureFile);
	if (!result)
		return false;

	return true;
}

void Model::LoadTextureObj(ID3D11ShaderResourceView* textureSrv)
{
	texture = new Texture;

	texture->SetTexture(textureSrv);
}


void Model::Shutdown()
{
	// Release the model textureSRV.
	ReleaseTexture();

	// Shutdown the vertex and index buffers.
	ShutdownBuffers();

	return;
}


void Model::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return;
}

void Model::ShutdownBuffers()
{
	// Release the index buffer.
	if(indexBuffer)
	{
		indexBuffer->Release();
		indexBuffer = 0;
	}

	// Release the vertex buffer.
	if(vertexBuffer)
	{
		vertexBuffer->Release();
		vertexBuffer = 0;
	}

	return;
}


void Model::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(Vertex); 
	offset = 0;
    
	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

    // Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}


bool Model::CreateCube(ID3D11Device* device)
{
	// Create own vertexbuffer and indices
	Vertex cube[] = 
	{
		// Front Face
		Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, -1.0f, -1.0f),
		Vertex(-1.0f,  1.0f, -1.0f, 0.0f, 0.0f,-1.0f,  1.0f, -1.0f),
		Vertex(1.0f,  1.0f, -1.0f, 1.0f, 0.0f, 1.0f,  1.0f, -1.0f),
		Vertex(1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f),

		// Back Face
		Vertex(-1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f),
		Vertex(1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, -1.0f, 1.0f),
		Vertex(1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,  1.0f, 1.0f),
		Vertex(-1.0f,  1.0f, 1.0f, 1.0f, 0.0f,-1.0f,  1.0f, 1.0f),

		// Top Face
		Vertex(-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 1.0f, -1.0f),
		Vertex(-1.0f, 1.0f,  1.0f, 0.0f, 0.0f, -1.0f, 1.0f,  1.0f),
		Vertex(1.0f, 1.0f,  1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  1.0f),
		Vertex(1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f),

		// Bottom Face
		Vertex(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, -1.0f),
		Vertex(1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, -1.0f, -1.0f),
		Vertex(1.0f, -1.0f,  1.0f, 0.0f, 0.0f, 1.0f, -1.0f,  1.0f),
		Vertex(-1.0f, -1.0f,  1.0f, 1.0f, 0.0f,-1.0f, -1.0f,  1.0f),

		// Left Face
		Vertex(-1.0f, -1.0f,  1.0f, 0.0f, 1.0f, -1.0f, -1.0f,  1.0f),
		Vertex(-1.0f,  1.0f,  1.0f, 0.0f, 0.0f, -1.0f,  1.0f,  1.0f),
		Vertex(-1.0f,  1.0f, -1.0f, 1.0f, 0.0f, -1.0f,  1.0f, -1.0f),
		Vertex(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, -1.0f),

		// Right Face
		Vertex(1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, -1.0f, -1.0f),
		Vertex(1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 1.0f,  1.0f, -1.0f),
		Vertex(1.0f,  1.0f,  1.0f, 1.0f, 0.0f, 1.0f,  1.0f,  1.0f),
		Vertex(1.0f, -1.0f,  1.0f, 1.0f, 1.0f, 1.0f, -1.0f,  1.0f),
	};

	DWORD cubeIndices[]
	{
		// Front Face
		0,  1,  2,
		0,  2,  3,

		// Back Face
		4,  5,  6,
		4,  6,  7,

		// Top Face
		8,  9, 10,
		8, 10, 11,

		// Bottom Face
		12, 13, 14,
		12, 14, 15,

		// Left Face
		16, 17, 18,
		16, 18, 19,

		// Right Face
		20, 21, 22,
		20, 22, 23
	};

	indexCount = ARRAYSIZE(cubeIndices);
	vertexCount = ARRAYSIZE(cube);

	// Vertexbuffer description for quad
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.CPUAccessFlags = 0u;
	bufferDesc.MiscFlags = 0u;
	bufferDesc.ByteWidth = sizeof(Vertex) * vertexCount;
	bufferDesc.StructureByteStride = sizeof(Vertex);

	D3D11_SUBRESOURCE_DATA resourceData;
	ZeroMemory(&resourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	resourceData.pSysMem = cube;

	// Create vertexbuffer for quad
	hr = device->CreateBuffer(&bufferDesc, &resourceData, &vertexBuffer);
	if (FAILED(hr)) {
		MessageBox(0, L"Failed to create vertex buffer", L"MODEL ERROR", MB_ICONERROR);
		return false;
	}

	// Indexbuffer description for quad
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.CPUAccessFlags = 0u;
	bufferDesc.MiscFlags = 0u;
	bufferDesc.ByteWidth = sizeof(DWORD) * indexCount;
	bufferDesc.StructureByteStride = sizeof(DWORD);

	ZeroMemory(&resourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	resourceData.pSysMem = cubeIndices;

	// Create indexbuffer for quad
	hr = device->CreateBuffer(&bufferDesc, &resourceData, &indexBuffer);
	if (FAILED(hr)) 
	{
		MessageBox(0, L"Failed to create index buffer", L"MODEL ERROR", MB_ICONERROR);
		return false;
	}

	return true;
}

bool Model::CreateQuad(ID3D11Device* device)
{
	// Create own vertexbuffer and indices
	Vertex quad[] = 
	{

		Vertex(-1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, -1.0f),
		Vertex(-1.0f,  1.0f, 0.0f, 0.0f, 0.0f,-1.0f,  1.0f, -1.0f),
		Vertex(1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 1.0f,  1.0f, -1.0f),
		Vertex(1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f),

	};

	DWORD quadIndices[] =
	{
		// Front Face
		0,  1,  2,
		0,  2,  3,

	};

	indexCount = ARRAYSIZE(quadIndices);
	vertexCount = ARRAYSIZE(quad);

	// Vertexbuffer description for quad
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.CPUAccessFlags = 0u;
	bufferDesc.MiscFlags = 0u;
	bufferDesc.ByteWidth = sizeof(Vertex) * vertexCount;
	bufferDesc.StructureByteStride = sizeof(Vertex);

	D3D11_SUBRESOURCE_DATA resourceData;
	ZeroMemory(&resourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	resourceData.pSysMem = quad;

	// Create vertexbuffer for quad
	hr = device->CreateBuffer(&bufferDesc, &resourceData, &vertexBuffer);
	if (FAILED(hr)) {
		MessageBox(0, L"Failed to create vertex buffer for quad!", L"MODEL ERROR", MB_OK);
		return false;
	}

	// Indexbuffer description for quad
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.CPUAccessFlags = 0u;
	bufferDesc.MiscFlags = 0u;
	bufferDesc.ByteWidth = sizeof(DWORD) * indexCount;
	bufferDesc.StructureByteStride = sizeof(DWORD);

	ZeroMemory(&resourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	resourceData.pSysMem = quadIndices;

	// Create indexbuffer for quad
	hr = device->CreateBuffer(&bufferDesc, &resourceData, &indexBuffer);
	if (FAILED(hr)) 
	{
		MessageBox(0, L"Failed to create index buffer for quad!", L"MODEL ERROR", MB_OK);
		return false;
	}

	return true;
}

bool Model::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, LPCWSTR filename)
{
	bool result;

	// Create the textureSRV object.
	texture = new Texture;
	if (!texture)
	{
		return false;
	}

	// Initialize the textureSRV object.
	result = texture->Initialize(device, filename);
	if (!result)
	{
		return false;
	}

	return true;
}


void Model::ReleaseTexture()
{
	// Release the textureSRV object.
	if (texture)
	{
		texture->Shutdown();
		delete texture;
		texture = 0;
	}

	return;
}