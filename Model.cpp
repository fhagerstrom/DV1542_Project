#include "Defines.h"
#include "Model.h"



Model::Model(ID3D11Device* device, std::string name, XMVECTOR position, XMVECTOR rotation, XMVECTOR scale)
	:transform(position, rotation, scale), name(name), mesh(device, name)
{
	Update();

	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = (int)mesh.faces.size() * sizeof(Face);
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = sizeof(Face);

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = mesh.faces.data();

	HRESULT hr = device->CreateBuffer(&desc, &data, &vBuffer);
	if (FAILED(hr))
	{
		std::cerr << "ERROR! COULD NOT CREATE MESH VERTEXBUFFER!" << std::endl;
	}
	desc.ByteWidth = mesh.vertexCount * sizeof(XMFLOAT3);
	desc.StructureByteStride = sizeof(XMFLOAT3);

	data.pSysMem = mesh.positions.data();
	hr = device->CreateBuffer(&desc, &data, &posBuffer);
	if (FAILED(hr))
	{
		std::cerr << "ERROR! COULD NOT CREATE POSITION VERTEXBUFFER" << std::endl;
	}
}

Model::~Model()
{

}

void Model::Update()
{
	XMMATRIX translation = XMMatrixTranslationFromVector(transform.translation);
	XMMATRIX rotation = XMMatrixRotationRollPitchYawFromVector(transform.rotation);
	XMMATRIX scale = XMMatrixScalingFromVector(transform.scale);
	worldMatrix = scale * rotation * translation;
}

int Model::GetVertexCount()
{
	return this->mesh.vertexCount; 
}

XMMATRIX Model::GetWorldMatrix()
{
	return this->worldMatrix; 
}

ID3D11Buffer** Model::GetMTLBuffer()
{
	return &this->mesh.mtlBuffer;
}

ID3D11Buffer** Model::GetPositionsBuffer()
{
	return &this->posBuffer;
}

ID3D11Buffer** Model::GetBuffer()
{
	return &this->vBuffer; 
}

XMVECTOR Model::GetPosition()
{
	return this->transform.translation; 
}

const Material::Data& Model::GetMaterial()
{
	return this->mesh.material.data;
}


void Model::BindTextures(ID3D11DeviceContext* context, int startSlot)
{
	int slot = startSlot;
	for (auto texture : mesh.material.diffuseTexures)
	{
		texture->Bind(context, slot);
		slot++;
	}
}

void Model::SetTranslation(XMVECTOR translation)
{
	this->transform.translation = translation;
}

void Model::SetRotation(XMVECTOR rotation)
{
	this->transform.rotation = rotation; 
}

void Model::SetScale(XMVECTOR scale)
{
	this->transform.scale = scale; 
}

XMVECTOR Model::GetRotation()
{ 
	return this->transform.rotation;
}

void Model::Shutdown()
{
	mesh.Shutdown();
	vBuffer->Release();
	posBuffer->Release();
}