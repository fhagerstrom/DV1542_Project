#include "Water.h"
#include "WICTextureLoader.h"


Water::Water()
{

}


Water::~Water()
{
	//release texture
	waterSRV->Release();

	//Release vertex and index buffer
	// Release the index buffer.
	if (iBuffer)
	{
		iBuffer->Release();
		iBuffer = 0;
	}

	// Release the vertex buffer.
	if (vBuffer)
	{
		vBuffer->Release();
		vBuffer = 0;
	}

	return;
}

bool Water::Initialize(ID3D11Device* device, const WCHAR* textureFile, float waterHeight, float waterRadius)
{
	// Store water height.
	this->waterHeight = waterHeight;

	// Initialize buffers.
	InitializeBuffers(device, waterRadius);

	// Load texture.
	LoadTexture(device, textureFile);

	// Set tiles for the normal map.
	normalMapTiling.x = 0.01f;
	normalMapTiling.y = 0.02f;

	// Initialize water translation.
	waterTranslation = 0.0f;

	return true;
}

bool Water::InitializeBuffers(ID3D11Device* device, float waterRadius)
{
	Vertex* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// Set number of vertices and indices in their respective arrays.
	vertexCount = 6;
	indexCount = 6;

	// Create vertex array.
	vertices = new Vertex[vertexCount];
	if (!vertices)
	{
		std::cerr << "Couldn't create vertex array for the water buffer!" << std::endl;
		return false;
	}

	// Create index array
	indices = new unsigned long[indexCount];
	if (!indices)
	{
		std::cerr << "Couldn't create index array for the water buffer!" << std::endl;
		return false;
	}

	// Load vertex array with data.
	vertices[0].position = dx::XMFLOAT3(-waterRadius, 0.15f, waterRadius);  // Top left.
	vertices[0].uv = dx::XMFLOAT2(0.0f, 0.0f);

	vertices[1].position = dx::XMFLOAT3(waterRadius, 0.15f, waterRadius);  // Top right.
	vertices[1].uv = dx::XMFLOAT2(1.0f, 0.0f);

	vertices[2].position = dx::XMFLOAT3(-waterRadius, 0.15f, -waterRadius);  // Bottom left.
	vertices[2].uv = dx::XMFLOAT2(0.0f, 1.0f);

	vertices[3].position = dx::XMFLOAT3(-waterRadius, 0.15f, -waterRadius);  // Bottom left.
	vertices[3].uv = dx::XMFLOAT2(0.0f, 1.0f);

	vertices[4].position = dx::XMFLOAT3(waterRadius, 0.15f, waterRadius);  // Top right.
	vertices[4].uv = dx::XMFLOAT2(1.0f, 0.0f);

	vertices[5].position = dx::XMFLOAT3(waterRadius, 0.15f, -waterRadius);  // Bottom right.
	vertices[5].uv = dx::XMFLOAT2(1.0f, 1.0f);

	// Load index array with data.
	indices[0] = 1;
	indices[1] = 2;
	indices[2] = 3;
	indices[3] = 4;
	indices[4] = 5;
	indices[5] = 6;

	// Vertex buffer description.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now finally create the vertex buffer.
	device->CreateBuffer(&vertexBufferDesc, &vertexData, &vBuffer);


	// Index buffer description.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	device->CreateBuffer(&indexBufferDesc, &indexData, &iBuffer);


	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;

}

bool Water::LoadTexture(ID3D11Device* device, const WCHAR* textureFile)
{
	dx::CreateWICTextureFromFile(device, textureFile, 0, &waterSRV);

	return true;
}

void Water::Update()
{
	// Movement update to simulate motion.
	waterTranslation += 0.01f;
	waterTranslation = waterTranslation + 0.01f;
	if (waterTranslation > 1.0f)
	{
		waterTranslation -= 1.0f;
	}

	return;
}

void Water::Render(ID3D11DeviceContext* context)
{
	RenderBuffers(context);
}

void Water::RenderBuffers(ID3D11DeviceContext* context)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(Vertex);
	offset = 0;

	// Tell the input assembler to set vertex buffers for rendering.
	context->IASetVertexBuffers(0, 1, &vBuffer, &stride, &offset);

	// Tell the input assembler to set index buffer for rendering.
	context->IASetIndexBuffer(iBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer.
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}