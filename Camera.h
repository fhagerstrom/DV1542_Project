#pragma once



class Camera
{
private:
	XMMATRIX viewMatrix;
	XMMATRIX perspectiveMatrix;
	XMVECTOR right;
	XMVECTOR up;
	XMVECTOR forward;
	XMVECTOR moveUp;
	XMFLOAT3 pos;
	float pitch;
	float yaw;
	float rotationSpeed;
	float speed;
	float viewDistance;
	float fov;
	float aspectRatio;
	float nearZ;
	float farZ;
	XMVECTOR direction;
public:
	Camera();
	Camera(float fov, float aspectRatio, float nearZ, float farZ, float rotationSpeed, float speed, XMFLOAT3 position);

	float GetNearZ();
	float GetFarZ();
	float GetRatio();
	float GetFov();
	XMVECTOR GetUpVector();
	XMFLOAT3 GetPosition();
	XMMATRIX GetViewMatrix();
	XMMATRIX GetPerspectiveMatrix();
	void MoveRight(float dt);
	void MoveForward(float dt);
	void MoveUpDown(float dt);
	void Rotate(float dx, float dy);
	void Update(float dt);
};