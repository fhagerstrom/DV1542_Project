#pragma once
#include <DirectXMath.h>

namespace dx = DirectX;

class Camera
{
private:
	// Camera matrices
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projMatrix;

	// Transformations
	DirectX::XMFLOAT3 startPos;
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT4 rotation;
	float xRotation;
	float yRotation;

public:
	Camera(float x, float y, float z);
	~Camera();

	// Transformations
	void MoveRelative(float x, float y, float z);
	void MoveAbsolute(float x, float y, float z);
	void Rotate(float x, float y);

	// Updating
	void Update(float dt);
	void UpdateViewMatrix();
	void UpdateProjMatrix(float aspectRatio);

	// Getter functions
	DirectX::XMFLOAT3 GetPosition()
	{
		return pos;
	}

	DirectX::XMFLOAT4X4 GetView()
	{
		return viewMatrix;
	}

	DirectX::XMFLOAT4X4 GetProjection()

	{
		return projMatrix;
	}
};

