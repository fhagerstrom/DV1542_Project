#include "Camera.h"


Camera::Camera()
{
	posX = 0.0f;
	posY = 0.0f;
	posZ = 0.0f;

	rotX = 0.0f;
	rotY = 0.0f;
	rotZ = 0.0f;
}

Camera::~Camera()
{
}


void Camera::SetPosition(float x, float y, float z)
{
	eyePosition.m128_f32[0] = x;
	eyePosition.m128_f32[1] = y;
	eyePosition.m128_f32[2] = z;
}


void Camera::SetRotation(float x, float y, float z)
{
	rotX = x;
	rotY = y;
	rotZ = z;
	return;
}

void Camera::SetTargetPosition(float x, float y, float z)
{
	this->lookAt = XMVectorSet(x, y, z, 1.0f);
}

void Camera::SetUpvector(float x, float y, float z)
{
	this->upVector = XMVectorSet(x, y, z, 0.0f);
}

DirectX::XMFLOAT3 Camera::GetLookAt()
{
	return XMFLOAT3(lookAt.m128_f32[0], lookAt.m128_f32[1], lookAt.m128_f32[2]);;
}


XMFLOAT3 Camera::GetPosition()
{
	return XMFLOAT3(eyePosition.m128_f32[0], eyePosition.m128_f32[1], eyePosition.m128_f32[2]);
}


XMFLOAT3 Camera::GetRotation()
{
	return XMFLOAT3(rotX, rotY, rotZ);
}

void Camera::GetInputs(Input* input)
{
	this->input = input;
	this->directInput = input->GetDirectInput8();
	this->keyboard = input->GetDirectInput8Keyboard();
	this->mouse = input->GetDirectInput8Mouse();
}

void Camera::DetectInputs(double deltaTime)
{
	DIMOUSESTATE mouseCurrState;
	BYTE keyboardState[256];

	keyboard->Acquire();
	mouse->Acquire();

	mouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrState);
	keyboard->GetDeviceState(sizeof(keyboardState), (LPVOID)&keyboardState);

	float moveSpeed = (float)(15.0f * deltaTime);
	float lookSpeed = (float)(5.0f * deltaTime);

	// LEFT
	if (keyboardState[DIK_A] & 0x80)
	{
		moveLeftRight -= moveSpeed;
	}

	// RIGHT
	if (keyboardState[DIK_D] & 0x80)
	{
		moveLeftRight += moveSpeed;
	}

	// FORWARDS
	if (keyboardState[DIK_W] & 0x80)
	{
		moveBackForward += moveSpeed;
	}

	// BACKWARDS
	if (keyboardState[DIK_S] & 0x80)
	{
		moveBackForward -= moveSpeed;
	}

	// ASCEND
	if (keyboardState[DIK_SPACE] & 0x80)
	{
		eyePosition += XMVectorSet(0.0f, moveSpeed, 0.0f, 1.0f);
	}

	// DESCEND
	if (keyboardState[DIK_LCONTROL] & 0x80)
	{
		eyePosition -= XMVectorSet(0.0f, moveSpeed, 0.0f, 1.0f);
	}

	// Reset camera position
	if (keyboardState[DIK_R] & 0x80)
	{
		SetPosition(0.0f, 10.0f, -50.0f);
		camYaw = 0.0f;
		camPitch = 0.0f;
	}

	//// LOOK AROUND (MOUSE)
	//if ((mouseCurrState.lX != mouseLastState.lX) || (mouseCurrState.lY != mouseLastState.lY))
	//{
	//	camYaw += mouseLastState.lX * 0.001f;
	//	camPitch += mouseCurrState.lY * 0.001f;

	//	mouseLastState = mouseCurrState;
	//}

	// LOKK AROUND (KEYBOARD)
	if (keyboardState[DIK_UPARROW] & 0x80)
	{
		camPitch -= lookSpeed;
	}

	if (keyboardState[DIK_DOWNARROW] & 0x80)
	{
		camPitch += lookSpeed;
	}

	if (keyboardState[DIK_RIGHTARROW] & 0x80)
	{
		camYaw += lookSpeed;
	}

	if (keyboardState[DIK_LEFTARROW] & 0x80)
	{
		camYaw -= lookSpeed;
	}

	UpdateCam();
}


void Camera::UpdateCam()
{
	// Set standard definition for main camera
	rotationMatrix = XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0.0f);
	lookAt = XMVector3TransformCoord(defaultForward, rotationMatrix);
	lookAt = XMVector3Normalize(lookAt);

	// Set new value based on mouse movement
	XMMATRIX RotateYTempMatrix = XMMatrixRotationY(camYaw);

	camRight = XMVector3TransformCoord(defaultRight, RotateYTempMatrix);
	upVector = XMVector3TransformCoord(upVector, RotateYTempMatrix);
	camForward = XMVector3TransformCoord(defaultForward, RotateYTempMatrix);

	// Move cameraposition to left and right based on keyboard and mouse inputs
	float camRightX = XMVectorGetX(camRight) * moveLeftRight;
	float camRightY = XMVectorGetY(camRight) * moveLeftRight;
	float camRightZ = XMVectorGetZ(camRight) * moveLeftRight;
	camRight = XMVectorSet(camRightX, camRightY, camRightZ, 0.0f);
	eyePosition += camRight;

	// Move camera position up and down based on keyboard and mouse inputs
	float camForwardX = XMVectorGetX(camForward) * moveBackForward;
	float camForwardY = XMVectorGetY(camForward) * moveBackForward;
	float camForwardZ = XMVectorGetZ(camForward) * moveBackForward;
	camForward = XMVectorSet(camForwardX, camForwardY, camForwardZ, 0.0f);
	eyePosition += camForward;

	// Reset values before next frame
	moveLeftRight = 0.0f;
	moveBackForward = 0.0f;

	lookAt = eyePosition + lookAt;

	// Create view matrix based of movment
	viewMatrix = XMMatrixLookAtLH(eyePosition, lookAt, upVector);
}