#pragma once

#include <DirectXMath.h>

#include "Mesh.h"
#include "Material.h"

namespace dx = DirectX;

class EngineEntity
{
private:
	Mesh* mesh;
	Material* mat;
	dx::XMFLOAT3 lightCol;
	dx::XMFLOAT4X4 worldMatrix;
	dx::XMFLOAT3 position;
	dx::XMFLOAT3 rotation;
	dx::XMFLOAT3 scale;

public:
	EngineEntity(Mesh* mesh, Material* material);
	// EngineEntity(Terrain* terrain, Material* material);
	EngineEntity(Mesh* mesh, dx::XMFLOAT3 lightColour);
	~EngineEntity();

	void UpdateWorldMatrix();

	void Move(float x, float y, float z) { position.x += x;	position.y += y; position.z += z; }
	void Rotate(float x, float y, float z) { rotation.x += x;	rotation.y += y;  rotation.z += z; }

	void SetPosition(float x, float y, float z) { position.x = x; position.y = y; position.z = z; }
	void SetRotation(float x, float y, float z) { rotation.x = x; rotation.y = y; rotation.z = z; }
	void SetScale(float x, float y, float z)	{ scale.x = x; scale.y = y; scale.z = z; }

	Mesh* GetMesh() { return mesh; }
	Material* GetMaterial() { return mat; }
	dx::XMFLOAT4X4 GetWorldMatrix() { return worldMatrix; }

	dx::XMFLOAT3 GetLightColour() { return lightCol; };
	dx::XMFLOAT3 GetPosition() { return position; };

};

