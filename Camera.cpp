#include "Camera.h"

Camera::Camera(float x, float y, float z)
{
	pos = dx::XMFLOAT3(x, y, z);
	startPos = dx::XMFLOAT3(x, y, z);
	dx::XMStoreFloat4(&rotation, dx::XMQuaternionIdentity());

	dx::XMStoreFloat4x4(&viewMatrix, dx::XMMatrixIdentity());
	dx::XMStoreFloat4x4(&projMatrix, dx::XMMatrixIdentity());
}
Camera::~Camera()
{

}

void Camera::MoveRelative(float x, float y, float z)
{
	dx::XMVECTOR direction = dx::XMVector3Rotate(dx::XMVectorSet(x, y, z, 0), XMLoadFloat4(&rotation));
	dx::XMStoreFloat3(&pos, direction);
}

void Camera::MoveAbsolute(float x, float y, float z)
{
	pos.x += x;
	pos.y += y;
	pos.z += z;
}

void Camera::Rotate(float x, float y)
{
	xRotation += x;
	yRotation += y;

	xRotation = max(min(xRotation, dx::XM_PIDIV2), -dx::XM_PIDIV2);

	// Recreate quaternion
	dx::XMStoreFloat4(&rotation, dx::XMQuaternionRotationRollPitchYaw(xRotation, yRotation, 0));
}


void Camera::UpdateViewMatrix()
{
	// Rotate the standard "forward" matrix by our rotation
	// This is the "look direction"
	dx::XMVECTOR direction = dx::XMVector3Rotate(dx::XMVectorSet(0, 0, 1, 0), XMLoadFloat4(&rotation));

	dx::XMMATRIX view = dx::XMMatrixLookToLH(XMLoadFloat3(&pos), direction, dx::XMVectorSet(0, 1, 0, 0));
	dx::XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(view));
}

void Camera::UpdateProjMatrix(float aspectRatio)
{
	dx::XMMATRIX perspective = dx::XMMatrixPerspectiveFovLH(
		0.25f * dx::XM_PI,		// FOV angle
		aspectRatio,			// Aspect ratio
		0.1f,					// Near clip plane distance
		1000.0f);				// Far clip plance distance

	dx::XMStoreFloat4x4(&projMatrix, dx::XMMatrixTranspose(perspective)); // Transpose for HLSL
}

// Update function, checking for key presses
void Camera::Update(float dt)
{
	// Camera speed
	float camSpeed = dt * 3;

	// Change speed with key presses
	if (GetAsyncKeyState(VK_SHIFT)) { camSpeed *= 2.5; }

	if (GetAsyncKeyState(VK_CONTROL)) { camSpeed *= 0.1f; }

	// Movement keys
	if (GetAsyncKeyState('W') & 0x8000) { MoveRelative(0, 0, camSpeed); }

	if (GetAsyncKeyState('S') & 0x8000) { MoveRelative(0, 0, -camSpeed); }

	if (GetAsyncKeyState('A') & 0x8000) { MoveRelative(-camSpeed, 0, 0); }

	if (GetAsyncKeyState('D') & 0x8000) { MoveRelative(camSpeed, 0, 0); }

	if (GetAsyncKeyState(VK_SPACE) & 0x8000) { MoveAbsolute(0, camSpeed, 0); }

	if (GetAsyncKeyState('C') & 0x8000) { MoveAbsolute(0, -camSpeed, 0); }

	// Reset camera position to start
	if (GetAsyncKeyState('R') & 0x8000)
	{
		pos = startPos;
		xRotation = 0;
		yRotation = 0;
		XMStoreFloat4(&rotation, dx::XMQuaternionIdentity());
	}

	// Update every frame - Check after optimizations on this
	UpdateViewMatrix();
}





