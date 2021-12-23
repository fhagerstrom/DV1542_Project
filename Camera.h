#pragma once



class Camera
{
private:
	XMMATRIX perspectiveMatrix;
	XMMATRIX viewMatrix;
	XMVECTOR forward;
	XMVECTOR right;
	XMVECTOR up;
	XMVECTOR moveUp;
	XMFLOAT3 position;

	float pitch;
	float yaw;
	float rotationSpeed;
	float speed;
	float viewDistance;

	float fov;
	float aspectRatio;
	float nearZ;
	float farZ;

	float pickingDistance;
	XMVECTOR direction;
public:
	Camera();
	Camera(float fov, float aspectRatio, float nearZ, float farZ, float rotationSpeed, float speed, XMFLOAT3 position);

	float GetNearZ() const { return this->nearZ; };
	float GetFarZ() const { return this->farZ; };
	float GetRatio() const { return this->aspectRatio; };
	float GetFov() const { return this->fov; };
	XMVECTOR GetUpVector() const { return this->up; };
	XMFLOAT3 GetPosition() const { return this->position; }
	XMMATRIX GetViewMatrix() const { return this->viewMatrix; };
	XMMATRIX GetPerspectiveMatrix() const { return this->perspectiveMatrix; };

	void MoveRight(float dt);
	void MoveForward(float dt);
	void MoveUpDown(float dt);
	void Rotate(float dx, float dy);

	void Update(float dt);
};