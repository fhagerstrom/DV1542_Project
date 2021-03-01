#include "Camera.h"

Camera::Camera()
{
	posX = 0.0f;
	posY = 0.0f;
	posZ = 0.0f;

	rotationX = 0;
	rotationY = 0.0f; 
	rotationZ = 0.0f;
}

Camera::Camera(const Camera&)
{

}

Camera::~Camera()
{

}

void Camera::SetPosition(float x, float y, float z)
{
	posX = x;
	posY = y;
	posZ = z;
	return;
}

void Camera::SetRotation(float x, float y, float z)
{
	rotationX = x;
	rotationY = y;
	rotationZ = z;
	return;
}

dx::XMFLOAT3 Camera::GetPosition()
{
	return dx::XMFLOAT3(posX, posY, posY);
}

dx::XMFLOAT3 Camera::GetRotation()
{
	return dx::XMFLOAT3(rotationX, rotationY, rotationZ);
}


void Camera::Render()
{
	float yaw, pitch, roll = 0.0f;
	dx::XMMATRIX rotationMatrix;

	dx::XMVECTOR up = dx::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	dx::XMVECTOR position = dx::XMVectorSet(posX, posY, posZ, 0.0f);
	dx::XMVECTOR lookAt = dx::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	// Set upward vector 
	/*
	dx::XMVectorSetX(up, 0.0f);
	dx::XMVectorSetY(up, 1.0f);
	dx::XMVectorSetZ(up, 0.0f);
	*/

	// Setup camera position
	/*
	dx::XMVectorSetX(position, posX);
	dx::XMVectorSetY(position, posY); 
	dx::XMVectorSetZ(position, posZ);

	// Set default camera view
	dx::XMVectorSetX(lookAt, 0.0f);
	dx::XMVectorSetY(lookAt, 0.0f);
	dx::XMVectorSetZ(lookAt, 1.0f);
	*/

	// Set yaw (x-axis), pitch (y-axis) and roll (z-axis) rotations in radians
	yaw = rotationX * (dx::XM_PI / 180);
	pitch = rotationY * (dx::XM_PI / 180);
	roll = rotationZ * (dx::XM_PI / 180);

	// Set rotation matrix to the pitch, yaw and roll values
	rotationMatrix = dx::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// Transform lookAt and up vectors by rotation matrix so rotation is correct at origin
	dx::XMVector3TransformCoord(lookAt, rotationMatrix);
	dx::XMVector3TransformCoord(up, rotationMatrix);

	// Translate rotated camera position to viewers location
	lookAt = dx::XMVectorAdd(position, lookAt);

	// Create view matrix from vectors
	viewMatrix = dx::XMMatrixLookAtLH(position, lookAt, up);
}

void Camera::GetViewMatrix(dx::XMMATRIX& viewMatrix)
{
	viewMatrix = this->viewMatrix;
	return;
}

void Camera::RenderBaseViewMatrix()
{
	float yaw, pitch, roll = 0.0f;
	dx::XMMATRIX rotationMatrix;

	dx::XMVECTOR up = dx::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	dx::XMVECTOR position = dx::XMVectorSet(posX, posY, posZ, 0.0f);
	dx::XMVECTOR lookAt = dx::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	/*
	// Set upward vector
	dx::XMVectorSetX(up, 0.0f);
	dx::XMVectorSetY(up, 1.0f);
	dx::XMVectorSetZ(up, 0.0f);

	// Set camera position
	dx::XMVectorSetX(position, posX);
	dx::XMVectorSetY(position, posY);
	dx::XMVectorSetZ(position, posZ);

	// Set default camera view
	dx::XMVectorSetX(lookAt, 0.0f);
	dx::XMVectorSetY(lookAt, 0.0f);
	dx::XMVectorSetZ(lookAt, 1.0f);
	*/

	// Set yaw (x-axis), pitch (y-axis) and roll (z-axis) rotations in radians
	yaw = rotationX * (dx::XM_PI / 180);
	pitch = rotationY * (dx::XM_PI / 180);
	roll = rotationZ * (dx::XM_PI / 180);

	// Set rotation matrix to the pitch, yaw and roll values
	rotationMatrix = dx::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// Transform lookAt and up vectors by rotation matrix so rotation is correct at origin
	dx::XMVector3TransformCoord(lookAt, rotationMatrix);
	dx::XMVector3TransformCoord(up, rotationMatrix);

	// Translate rotated camera position to viewers location
	lookAt = dx::XMVectorAdd(position, lookAt);

	// Create view matrix from vectors
	baseViewMatrix = dx::XMMatrixLookAtLH(position, lookAt, up);
}

void Camera::GetBaseViewMatrix(dx::XMMATRIX& viewMatrix)
{
	viewMatrix = this->baseViewMatrix;
	return;
}



