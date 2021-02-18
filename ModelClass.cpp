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
	return idxCount;
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


}