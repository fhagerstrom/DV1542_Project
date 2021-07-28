#include "EngineEntity.h"

namespace dx = DirectX;

EngineEntity::EngineEntity(Mesh* mesh, Material* material)
{
	this->mesh = mesh;
	this->material = material;

	XMStoreFloat4x4(&worldMatrix, dx::XMMatrixIdentity());
	position = dx::XMFLOAT3(0, 0, 0);
	rotation = dx::XMFLOAT3(0, 0, 0);
	scale = dx::XMFLOAT3(1, 1, 1);
}

EngineEntity::EngineEntity(Mesh* mesh, dx::XMFLOAT3 lightColor)
{
	this->mesh = mesh;
	this->lightColor = lightColor;

	XMStoreFloat4x4(&worldMatrix, dx::XMMatrixIdentity());
	position = dx::XMFLOAT3(0, 0, 0);
	rotation = dx::XMFLOAT3(0, 0, 0);
	scale = dx::XMFLOAT3(1, 1, 1);
}

EngineEntity::~EngineEntity()
{

}	

void EngineEntity::UpdateWorldMatrix()
{
	dx::XMMATRIX translation = dx::XMMatrixTranslation(position.x, position.y, position.z);
	dx::XMMATRIX rotationX = dx::XMMatrixRotationX(rotation.x);
	dx::XMMATRIX rotationY = dx::XMMatrixRotationY(rotation.y);
	dx::XMMATRIX rotationZ = dx::XMMatrixRotationZ(rotation.z);
	dx::XMMATRIX scaling = dx::XMMatrixScaling(scale.x, scale.y, scale.z);

	dx::XMMATRIX total = scaling * rotationZ * rotationY * rotationX * translation;
	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(total));
}
