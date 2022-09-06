#pragma once

#include <DirectXMath.h>

#include "Input.h"
using namespace DirectX;

class Camera
{
public:
	Camera();
	~Camera();

	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);
	void SetTargetPosition(float x, float y, float z);
	void SetUpvector(float x, float y, float z);

	DirectX::XMFLOAT3 GetLookAt();

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();

	void GetInputs(Input* input);
	void DetectInputs(double deltaTime);

	void SetViewMatrix(XMVECTOR eyePos, XMVECTOR lookAt, XMVECTOR upVector) { this->viewMatrix = XMMatrixLookAtLH(eyePos, lookAt, upVector); }
	void GetViewMatrix(XMMATRIX& viewMatrix) { viewMatrix = this->viewMatrix; }

	void UpdateCam();

private:
	float posX, posY, posZ;
	float rotX, rotY, rotZ;
	float yaw, pitch, roll;

	Input* input;
	IDirectInput8* directInput;
	IDirectInputDevice8* keyboard;
	IDirectInputDevice8* mouse;

	/*
	  DIMOUSESTATE describes the state of a mouse device that has up to four buttons,
	  or another device that is being accessed as if it were a mouse device.
	*/
	DIMOUSESTATE mouseLastState;

	XMMATRIX viewMatrix;
	XMVECTOR eyePosition = DirectX::XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f);
	XMVECTOR lookAt = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR upVector = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMVECTOR defaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	XMVECTOR defaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR defaultUpward = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR camForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	XMVECTOR camRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	XMMATRIX rotationMatrix;


	// Mouse x and y - axis position
	float camYaw = 0.0f;
	float camPitch = 0.0f;

	// Movement related variables
	float moveLeftRight = 0.0f;
	float moveBackForward = 0.0f;
};