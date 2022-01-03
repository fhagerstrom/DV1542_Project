#include "Defines.h"
#include "Frustum.h"

Frustum::Frustum()
{
}

Frustum::Frustum(const Frustum& other)
{
}

Frustum::~Frustum()
{
}

void Frustum::ConstructFrustum(float screenDepth, XMMATRIX projMatrix, XMMATRIX viewMatrix)
{
	float zMinimum, r;
	XMMATRIX matrix;
	XMFLOAT4X4 tempMatrix;
	XMStoreFloat4x4(&tempMatrix, projMatrix);


	// Calculate the minimum Z distance in the frustum.
	zMinimum = -tempMatrix._43 / tempMatrix._33;
	r = screenDepth / (screenDepth - zMinimum);
	tempMatrix._33 = r;
	tempMatrix._43 = -r * zMinimum;

	projMatrix = XMLoadFloat4x4(&tempMatrix);

	// Create the frustum matrix from the view matrix and updated projection matrix.
	matrix = XMMatrixMultiply(viewMatrix, projMatrix);
	XMStoreFloat4x4(&tempMatrix, matrix);

	XMVECTOR tempPlane;

	// Calculate near plane of frustum.
	planes[0].x = tempMatrix._14 + tempMatrix._13;
	planes[0].y = tempMatrix._24 + tempMatrix._23;
	planes[0].z = tempMatrix._34 + tempMatrix._33;
	planes[0].w = tempMatrix._44 + tempMatrix._43;
	tempPlane = XMLoadFloat4(&planes[0]);
	tempPlane = XMPlaneNormalize(tempPlane);
	XMStoreFloat4(&planes[0], tempPlane);

	// Calculate far plane of frustum.
	planes[1].x = tempMatrix._14 - tempMatrix._13;
	planes[1].y = tempMatrix._24 - tempMatrix._23;
	planes[1].z = tempMatrix._34 - tempMatrix._33;
	planes[1].w = tempMatrix._44 - tempMatrix._43;
	tempPlane = XMLoadFloat4(&planes[1]);
	tempPlane = XMPlaneNormalize(tempPlane);
	XMStoreFloat4(&planes[1], tempPlane);

	// Calculate left plane of frustum.
	planes[2].x = tempMatrix._14 + tempMatrix._11;
	planes[2].y = tempMatrix._24 + tempMatrix._21;
	planes[2].z = tempMatrix._34 + tempMatrix._31;
	planes[2].w = tempMatrix._44 + tempMatrix._41;
	tempPlane = XMLoadFloat4(&planes[2]);
	tempPlane = XMPlaneNormalize(tempPlane);
	XMStoreFloat4(&planes[2], tempPlane);

	// Calculate right plane of frustum.
	planes[3].x = tempMatrix._14 - tempMatrix._11;
	planes[3].y = tempMatrix._24 - tempMatrix._21;
	planes[3].z = tempMatrix._34 - tempMatrix._31;
	planes[3].w = tempMatrix._44 - tempMatrix._41;
	tempPlane = XMLoadFloat4(&planes[3]);
	tempPlane = XMPlaneNormalize(tempPlane);
	XMStoreFloat4(&planes[3], tempPlane);

	// Calculate top plane of frustum.
	planes[4].x = tempMatrix._14 - tempMatrix._12;
	planes[4].y = tempMatrix._24 - tempMatrix._22;
	planes[4].z = tempMatrix._34 - tempMatrix._32;
	planes[4].w = tempMatrix._44 - tempMatrix._42;
	tempPlane = XMLoadFloat4(&planes[4]);
	tempPlane = XMPlaneNormalize(tempPlane);
	XMStoreFloat4(&planes[4], tempPlane);

	// Calculate bottom plane of frustum.
	planes[5].x = tempMatrix._14 + tempMatrix._12;
	planes[5].y = tempMatrix._24 + tempMatrix._22;
	planes[5].z = tempMatrix._34 + tempMatrix._32;
	planes[5].w = tempMatrix._44 + tempMatrix._42;
	tempPlane = XMLoadFloat4(&planes[5]);
	tempPlane = XMPlaneNormalize(tempPlane);
	XMStoreFloat4(&planes[5], tempPlane);


	return;
}
/*
bool Frustum::CheckPoint(float x, float y, float z)
{
	XMVECTOR tempPlane = XMVectorSet(0, 0, 0, 0);
	XMVECTOR tempVec = XMVectorSet(0, 0, 0, 0);
	float tempFloat;
	XMFLOAT3 tmp = XMFLOAT3(x, y, z);
	XMStoreFloat3(&tmp, tempVec);
	// Check if the point is inside all six planes of the view frustum.
	for (int i = 0; i < 6; i++)
	{
		tempPlane = XMLoadFloat4(&planes[i]);
		XMStoreFloat(&tempFloat, XMPlaneDotCoord(tempPlane, tempVec));
		if (tempFloat < 0.0f)
		{
			return false;
		}
	}
	return true;
}
*/
bool Frustum::CheckCube(float xCenter, float yCenter, float zCenter, float radius)
{
	XMVECTOR tempPlane = XMVectorSet(0, 0, 0, 0);
	XMVECTOR tempPlane2 = XMVectorSet(0, 0, 0, 0);
	float tempFloat;
	XMFLOAT3 tempFloat3 = XMFLOAT3(0.0f, 0.0f, 0.0f);
	// Check if any one point of the cube is in the view frustum.
	for (int i = 0; i < 6; i++)
	{
		tempPlane = XMLoadFloat4(&planes[i]);
		tempFloat3 = XMFLOAT3((xCenter - radius), (yCenter - radius), (zCenter - radius));
		tempPlane2 = XMLoadFloat3(&tempFloat3);
		tempPlane = XMPlaneDotCoord(tempPlane, tempPlane2);
		XMStoreFloat(&tempFloat, tempPlane);
		if(tempFloat >= 0.0f)
		{
			continue;
		}
		
		tempPlane = XMLoadFloat4(&planes[i]);
		tempFloat3 = XMFLOAT3((xCenter + radius), (yCenter - radius), (zCenter - radius));
		tempPlane2 = XMLoadFloat3(&tempFloat3);
		tempPlane = XMPlaneDotCoord(tempPlane, tempPlane2);
		XMStoreFloat(&tempFloat, tempPlane);
		if(tempFloat >= 0.0f)
		{
			continue;
		}

		
		tempPlane = XMLoadFloat4(&planes[i]);
		tempFloat3 = XMFLOAT3((xCenter - radius), (yCenter + radius), (zCenter - radius));
		tempPlane2 = XMLoadFloat3(&tempFloat3);
		tempPlane = XMPlaneDotCoord(tempPlane, tempPlane2);
		XMStoreFloat(&tempFloat, tempPlane);
		if(tempFloat >= 0.0f)
		{
			continue;
		}

		tempPlane = XMLoadFloat4(&planes[i]);
		tempFloat3 = XMFLOAT3((xCenter + radius), (yCenter + radius), (zCenter - radius));
		tempPlane2 = XMLoadFloat3(&tempFloat3);
		tempPlane = XMPlaneDotCoord(tempPlane, tempPlane2);
		XMStoreFloat(&tempFloat, tempPlane);
		if(tempFloat >= 0.0f)
		{
			continue;
		}

		
		 
		 tempPlane = XMLoadFloat4(&planes[i]);
		 tempFloat3 = XMFLOAT3((xCenter - radius), (yCenter - radius), (zCenter + radius));
		 tempPlane2 = XMLoadFloat3(&tempFloat3);
		 tempPlane = XMPlaneDotCoord(tempPlane, tempPlane2);
		 XMStoreFloat(&tempFloat, tempPlane);
		 if(tempFloat >= 0.0f)
		 {
			continue;
		 }
		
		
		tempPlane = XMLoadFloat4(&planes[i]);
		tempFloat3 = XMFLOAT3((xCenter + radius), (yCenter - radius), (zCenter + radius));
		tempPlane2 = XMLoadFloat3(&tempFloat3);
		tempPlane = XMPlaneDotCoord(tempPlane, tempPlane2);
		XMStoreFloat(&tempFloat, tempPlane);
		if(tempFloat >= 0.0f)
		{
			continue;
		}

		
		
		tempPlane = XMLoadFloat4(&planes[i]);
		tempFloat3 = XMFLOAT3((xCenter - radius), (yCenter + radius), (zCenter + radius));
		tempPlane2 = XMLoadFloat3(&tempFloat3);
		tempPlane = XMPlaneDotCoord(tempPlane, tempPlane2);
		XMStoreFloat(&tempFloat, tempPlane);
		if(tempFloat >= 0.0f)
		{
			continue;
		}
		
		
		tempPlane = XMLoadFloat4(&planes[i]);
		tempFloat3 = XMFLOAT3((xCenter + radius), (yCenter + radius), (zCenter + radius));
		tempPlane2 = XMLoadFloat3(&tempFloat3);
		tempPlane = XMPlaneDotCoord(tempPlane, tempPlane2);
		XMStoreFloat(&tempFloat, tempPlane);
		if(tempFloat >= 0.0f)
		{
			continue;
		}

		return false;  //not in frustum
	}

	return true;  //in frustum
}
/*
bool Frustum::CheckSphere(float xCenter, float yCenter, float zCenter, float radius)
{
	return false;
}

bool Frustum::CheckRectangle(float xCenter, float yCenter, float zCenter, float xSize, float ySize, float zSize)
{
	return false;
}
*/