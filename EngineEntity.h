#pragma once

#include <DirectXMath.h>

#include "Mesh.h"
#include "Material.h"

namespace dx = DirectX;

class EngineEntity
{
private:
	Mesh* mesh;
	Material* material;
	dx::XMFLOAT4X4 worldMatrix;
	dx::XMFLOAT3 lightColor;
	dx::XMFLOAT3 position;
	dx::XMFLOAT3 rotation;
	dx::XMFLOAT3 scale;

public:
	EngineEntity(Mesh* mesh, Material* material);
	EngineEntity(Mesh* mesh, dx::XMFLOAT3 lightColor);
	~EngineEntity();

	void UpdateWorldMatrix();

	void Move(float x, float y, float z) { position.x += x;	position.y += y; position.z += z; }
	void Rotate(float x, float y, float z) { rotation.x += x;	rotation.y += y;  rotation.z += z; }

	void SetPosition(float x, float y, float z) { position.x = x; position.y = y; position.z = z; }
	void SetRotation(float x, float y, float z) { rotation.x = x; rotation.y = y; rotation.z = z; }
	void SetScale(float x, float y, float z)	{ scale.x = x; scale.y = y; scale.z = z; }

	Mesh* GetMesh() { return mesh; }
	Material* GetMaterial() { return material; }
	dx::XMFLOAT4X4 GetWorldMatrix() { return worldMatrix; }

	dx::XMFLOAT3 GetLightColor() { return lightColor; };
	dx::XMFLOAT3 GetPosition() { return position; };

};

