#pragma once
#include "Mesh.h"

class Model
{
private:

	Mesh mesh;
	XMMATRIX worldMatrix; //world matrix is unique for each object in the world. It's responsible for transforming vertices of objects from its
	//local space to world space
	std::string name;

	// Struct for storing data of models
	struct Transform
	{
		XMVECTOR translation;
		XMVECTOR rotation;
		XMVECTOR scale;
		Transform(XMVECTOR translation, XMVECTOR  rotation, XMVECTOR scale)
			: translation(translation), rotation(rotation), scale(scale) {}

	}transform;

public:

	Model() = default;
	Model(ID3D11Device* device, std::string name, XMVECTOR position = { 0.0f,0.0f,0.0f },
		XMVECTOR rotation = { 0.0f,0.0f,0.0f }, XMVECTOR scale = { 1.0f, 1.0f, 1.0f });
	~Model();

	// Buffers
	ID3D11Buffer* vBuffer;
	ID3D11Buffer* posBuffer;

	void Update();//updates data of models
	int GetVertexCount(); 
	XMMATRIX GetWorldMatrix(); 
	ID3D11Buffer** GetMTLBuffer();
	ID3D11Buffer** GetPositionsBuffer();
	ID3D11Buffer** GetBuffer();
	XMVECTOR GetPosition();
	const Material::Data& GetMaterial();
	void BindTextures(ID3D11DeviceContext* context, int startSlot = 0);
	void SetTranslation(XMVECTOR translation);
	void SetRotation(XMVECTOR rotation);
	void SetScale(XMVECTOR scale);
	XMVECTOR GetRotation();
	void Shutdown();
};