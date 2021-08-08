#pragma once
#include <Windows.h>
#include <DirectXMath.h>
#include <iostream>

#include "DXInit.h"

class Camera
{
private:
	// Camera matrices
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projMatrix;

	// Transformations
	DirectX::XMFLOAT3 startPosition;
	DirectX::XMFLOAT3 position;
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
	void Update(float deltaTime);
	void ResetCamera();
	void UpdateViewMatrix();
	void UpdateProjectionMatrix(float aspectRatio);

	// Getters
	DirectX::XMFLOAT3 GetPosition() { return position; }
	DirectX::XMFLOAT4X4 GetView() { return viewMatrix; }
	DirectX::XMFLOAT4X4 GetProjection() { return projMatrix; }
};

