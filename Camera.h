#pragma once
#include <DirectXMath.h>
#include <Windows.h>

namespace dx = DirectX;

class Camera
{
private:
	// Camera matrices
	dx::XMFLOAT4X4 viewMatrix;
	dx::XMFLOAT4X4 projMatrix;

	// Transformations
	dx::XMFLOAT3 startPos;
	dx::XMFLOAT3 pos;
	dx::XMFLOAT4 rotation;
	float xRotation = 0.0f;
	float yRotation = 0.0f;

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

