#include "Camera.h"
#include <Windows.h>

namespace dx = DirectX;


Camera::Camera(float x, float y, float z)
{
	position = dx::XMFLOAT3(x, y, z);
	startPosition = dx::XMFLOAT3(x, y, z);
	XMStoreFloat4(&rotation, dx::XMQuaternionIdentity());
	xRotation = 0;
	yRotation = 0;

	XMStoreFloat4x4(&viewMatrix, dx::XMMatrixIdentity());
	XMStoreFloat4x4(&projMatrix, dx::XMMatrixIdentity());
}

Camera::~Camera()
{ }


void Camera::MoveRelative(float x, float y, float z)
{
	
	dx::XMVECTOR direction = dx::XMVector3Rotate(dx::XMVectorSet(x, y, z, 0), XMLoadFloat4(&rotation));


	XMStoreFloat3(&position, dx::XMVectorAdd(XMLoadFloat3(&position), direction));
}


void Camera::MoveAbsolute(float x, float y, float z)
{
	
	position.x += x;
	position.y += y;
	position.z += z;
}


void Camera::Rotate(float x, float y)
{
	
	xRotation += x;
	yRotation += y;

	xRotation = max(min(xRotation, dx::XM_PIDIV2), -dx::XM_PIDIV2);

	// Recreate the quaternion
	XMStoreFloat4(&rotation, dx::XMQuaternionRotationRollPitchYaw(xRotation, yRotation, 0));
}

// Camera's update, which looks for key presses
void Camera::Update(float dt)
{
	// Current speed
	float speed = dt * 3;

	// Speed up or down
	if (GetAsyncKeyState(VK_SHIFT)) { speed *= 5; }
	if (GetAsyncKeyState(VK_CONTROL)) { speed *= 0.1f; }

	// Movement
	if (GetAsyncKeyState('W') & 0x8000) { MoveRelative(0, 0, speed); }
	if (GetAsyncKeyState('S') & 0x8000) { MoveRelative(0, 0, -speed); }
	if (GetAsyncKeyState('A') & 0x8000) { MoveRelative(-speed, 0, 0); }
	if (GetAsyncKeyState('D') & 0x8000) { MoveRelative(speed, 0, 0); }
	if (GetAsyncKeyState('X') & 0x8000) { MoveAbsolute(0, -speed, 0); }
	if (GetAsyncKeyState(' ') & 0x8000) { MoveAbsolute(0, speed, 0); }

	// Check for reset
	if (GetAsyncKeyState('R') & 0x8000)
	{
		position = startPosition;
		xRotation = 0;
		xRotation = 0;
		XMStoreFloat4(&rotation, dx::XMQuaternionIdentity());
	}

	// Update the view every frame - Hitta något mer optimiserat sätt att göra detta på WAAAAA
	UpdateViewMatrix();
}

// Creates a new view matrix based on current position and orientation
void Camera::UpdateViewMatrix()
{
	// Rotate the standard "forward" matrix by our rotation
	// This gives us our "look direction"
	dx::XMVECTOR direction = dx::XMVector3Rotate(dx::XMVectorSet(0, 0, 1, 0), XMLoadFloat4(&rotation));

	dx::XMMATRIX view = dx::XMMatrixLookToLH(XMLoadFloat3(&position), direction, dx::XMVectorSet(0, 1, 0, 0));

	XMStoreFloat4x4(&viewMatrix, dx::XMMatrixTranspose(view));
}

// Updates the projection matrix
void Camera::UpdateProjectionMatrix(float aspectRatio)
{
	dx::XMMATRIX p = dx::XMMatrixPerspectiveFovLH(
		0.25f * dx::XM_PI,		// Field of View Angle
		aspectRatio,		// Aspect ratio for window
		0.1f,				// Near clip plane distance
		100.0f);			// Far clip plane distance
		XMStoreFloat4x4(&projMatrix, XMMatrixTranspose(p)); // Transpose for HLSL!
}
