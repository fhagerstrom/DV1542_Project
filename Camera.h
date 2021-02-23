#pragma once
#include <DirectXMath.h>
#include <Windows.h>

namespace dx = DirectX;

class Camera
{
private:
	float posX, posY, posZ;
	float rotationX, rotationY, rotationZ;
	dx::XMMATRIX viewMatrix;
	dx::XMMATRIX baseViewMatrix;

public:
	Camera();
	Camera(const Camera&);
	~Camera();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);
	void Render();
	void GetViewMatrix(dx::XMMATRIX&);
	void RenderBaseViewMatrix();
	void GetBaseViewMatrix(dx::XMMATRIX&);

	dx::XMFLOAT3 GetPosition();
	dx::XMFLOAT3 GetRotation();
};

