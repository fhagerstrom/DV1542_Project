#include "EngineEntity.h"

EngineEntity::EngineEntity(Mesh* mesh, Material* material)
{
	meshEntity = mesh;
	materialEntity = material;

	position = dx::XMFLOAT3(0, 0, 0);
	rotation = dx::XMFLOAT3(0, 0, 0);
	scale = dx::XMFLOAT3(1, 1, 1);
	dx::XMStoreFloat4x4(&worldMatrix, dx::XMMatrixIdentity());
}

EngineEntity::EngineEntity(Mesh* mesh, dx::XMFLOAT3 lightColour)
{
	meshEntity = mesh;
	lightCol = lightColour;

	position = dx::XMFLOAT3(0, 0, 0);
	rotation = dx::XMFLOAT3(0, 0, 0);
	scale = dx::XMFLOAT3(1, 1, 1);
	dx::XMStoreFloat4x4(&worldMatrix, dx::XMMatrixIdentity());
}

EngineEntity::~EngineEntity()
{

}

void EngineEntity::UpdateWorldMatrix()
{
	dx::XMMATRIX translation = dx::XMMatrixTranslation(position.x, position.y, position.z);
	dx::XMMATRIX rotateX = dx::XMMatrixRotationX(rotation.x);
	dx::XMMATRIX rotateY = dx::XMMatrixRotationY(rotation.y);
	dx::XMMATRIX rotateZ = dx::XMMatrixRotationZ(rotation.z);
	dx::XMMATRIX scaling = dx::XMMatrixScaling(scale.x, scale.y, scale.z);

	dx::XMMATRIX total = scaling * rotateZ * rotateY * rotateX * translation;
	dx::XMStoreFloat4x4(&worldMatrix, dx::XMMatrixTranspose(total));
}