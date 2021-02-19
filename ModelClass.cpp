#include "ModelClass.h"

ModelClass::ModelClass()
{
	vertexBuffer = nullptr;
	indexBuffer = nullptr;
	texture = nullptr;
	model = nullptr;
}

ModelClass::ModelClass(const ModelClass& other)
{

}


ModelClass::~ModelClass()
{

}

bool ModelClass::Initialize(ID3D11Device* device, char* modelFilename, WCHAR* textureFilename)
{
	bool result;

	// Load model
	result = LoadModel(modelFilename);
	if (!result)
	{
		std::cerr << modelFilename << " Couldn't load!" << std::endl;
		return false;
	}

	// Initialize vertex and index buffers
	result = InitializeBuffers(device);
	if (!result)
	{
		std::cerr << "Couldn't initialize buffers!" << std::endl;
		return false;
	}

	// Load textures
	result = LoadTexture(device, textureFilename);
	if (!result)
	{
		std::cerr << textureFilename << " Couldn't load!" << std::endl;
	}

	return true;

}

void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return;
}

int ModelClass::GetIndexCount()
{
	return indexCount;
}

ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return texture->GetTexture();
}

bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT hr;

	// Vertex array
	vertices = new VertexType[vertexCount];
	if (!vertices)
	{
		std::cerr << "Couldn't create vertice array!" << std::endl;
		return false;
	}

	// Index array
	indices = new unsigned long[indexCount];
	if (!indices)
	{
		return false;
	}

	// Load vertex data
	for (int i = 0; i < vertexCount; ++i)
	{
		vertices[i].position = dx::XMFLOAT3(model[i].x, model[i].y, model[i].z);
		vertices[i].texture = dx::XMFLOAT2(model[i].tu, model[i].tv);
		vertices[i].normal = dx::XMFLOAT3(model[i].nx, model[i].ny, model[i].nz);
	}

	// Vertex buffer description
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give subresource struct a pointer to vertex data
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Create vertex buffer
	hr = device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
	if (FAILED(hr))
	{
		std::cerr << "Couldn't create vertex buffer!" << std::endl;
		return false;
	}

	// Index buffer description
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Sub reosurces, same as vertex
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create index buffer
	hr = device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);
	if (FAILED(hr))
	{
		std::cerr << "Couldn't create index buffer!" << std::endl;
		return false;
	}

	// Release arrays after creation
	delete[] vertices;
	delete[] indices;

	return true;
}

void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(VertexType);
	offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	return;
}

bool ModelClass::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	bool result;

	// Create texture object
	texture = new Texture;
	if (!texture)
	{
		std::cerr << "Couldn't create texture for " << filename << std::endl;
		return false;
	}

	// Initialize
	result = texture->Initialize(device, filename);
	if (!result)
	{
		std::cerr << "Couldn't initialize texture!" << std::endl;
		return false;
	}

	return true;
}

bool ModelClass::LoadModel(char* filename)
{
	// Find a better way to read the file than this. Quite cumbersome / complicated
	std::ifstream fin;
	char input;

	fin.open(filename);

	if (!fin)
	{
		std::cout << "Couldn't open / find file!" << std::endl;
		return false;
	}

	fin.get(input);

	// Read up until vertex count
	while (input != ':')
	{
		fin.get(input);
	}

	fin >> vertexCount;

	indexCount = vertexCount;

	model = new ModelType[vertexCount];
	if (!model)
	{
		std::cerr << "Couldn't load model!" << std::endl;
		return false;
	}

	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	fin.get(input);
	fin.get(input);

	for (int i = 0; i < vertexCount; ++i)
	{
		fin >> model[i].x >> model[i].y >> model[i].z;
		fin >> model[i].tu >> model[i].tv;
		fin >> model[i].nx >> model[i].ny >> model[i].nz;
	}

	fin.close();
	return true;
}

// RELEASE OBJECTS AFTERWARDS

