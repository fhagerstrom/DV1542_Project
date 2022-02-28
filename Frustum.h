#pragma once
class Frustum
{
public:
	Frustum();
	Frustum(const Frustum& other);
	~Frustum();

	void ConstructFrustum(float screenDepth, XMMATRIX projMatrix, XMMATRIX viewMatrix);

	//bool CheckPoint(float x, float y, float z);
	bool CheckCube(float xCenter, float yCenter, float zCenter, float radius);
	//bool CheckSphere(float xCenter, float yCenter, float zCenter, float radius);
	//bool CheckRectangle(float xCenter, float yCenter, float zCenter, float xSize, float ySize, float zSize); //instead of 1 radius as cube, has all dimensions

private:
	XMFLOAT4 planes[6]; //or XMVECTOR, depending on what works. 

};
