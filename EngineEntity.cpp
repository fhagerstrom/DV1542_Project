#include "EngineEntity.h"

using namespace DirectX;

EngineEntity::EngineEntity(Mesh* mesh, Material* material)
{
	this->mesh = mesh;
	mat = material;

	XMStoreFloat4x4(&worldMatrix, XMMatrixIdentity());
	position = XMFLOAT3(0, 0, 0);
	rotation = XMFLOAT3(0, 0, 0);
	scale = XMFLOAT3(1, 1, 1);
}

EngineEntity::EngineEntity(Mesh* mesh, DirectX::XMFLOAT3 lightColour)
{
	this->mesh = mesh;
	lightCol = lightColour;

	XMStoreFloat4x4(&worldMatrix, XMMatrixIdentity());
	position = XMFLOAT3(0, 0, 0);
	rotation = XMFLOAT3(0, 0, 0);
	scale = XMFLOAT3(1, 1, 1);
}

EngineEntity::~EngineEntity(void)	
{
}

void EngineEntity::UpdateWorldMatrix()
{
	XMMATRIX trans = XMMatrixTranslation(position.x, position.y, position.z);
	XMMATRIX rotationX = XMMatrixRotationX(rotation.x);
	XMMATRIX rotationY = XMMatrixRotationY(rotation.y);
	XMMATRIX rotationZ = XMMatrixRotationZ(rotation.z);
	XMMATRIX scaling = XMMatrixScaling(scale.x, scale.y, scale.z);

	XMMATRIX total = scaling * rotationZ * rotationY * rotationX * trans;
	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(total));
}

DirectX::XMFLOAT3 EngineEntity::GetLightColour()
{
	return lightCol;
}

DirectX::XMFLOAT3 EngineEntity::GetPosition()
{
	return position;
}
