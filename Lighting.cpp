#include "Defines.h"
#include "Lighting.h"

Lighting::Lighting(float range)
{
	data.range = range;
	
	XMMATRIX ortographicMatrix = XMMatrixOrthographicOffCenterLH(-range, range, -range, range, -range, range * 8.0f);
	//this builds a custom orthogonal projection matrix, left handed system

	XMVECTOR direction = { 0.75f, 0.65f, 0 };
	XMVector3Normalize(direction);
	//this function returns normalized version of 3d vector

	XMVECTOR position = direction * data.range;	
	//goes from origo through direction. The distance is range

	XMStoreFloat3(&data.direction, direction);
	XMMATRIX viewMatrix = XMMatrixLookAtLH(position, { 0,0,0 }, { 0 ,1 ,0 });
	matrix = XMMatrixTranspose(viewMatrix * ortographicMatrix);
}

Lighting::~Lighting()
{

}

XMMATRIX Lighting::GetMatrix() const
{ 
	return this->matrix; 
} 