#include <d3d11.h>
#include <DirectXMath.h>
#include <fstream>

#include "Texture.h"

namespace dx = DirectX;


class ModelClass
{
private:
	struct VertexType
	{
		dx::XMFLOAT3 position;
		dx::XMFLOAT2 texture;
		dx::XMFLOAT3 normal;
	};

	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	ID3D11Buffer* vertexBuffer, *indexBuffer;
	int vertexCount, indexCount;
	Texture* texture;
	ModelType* model;

	bool InitializeBuffers(ID3D11Device*);
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device*, const WCHAR*);
	bool LoadModel(const char*);

	void ShutdownBuffers();
	void ReleaseModel();
	void ReleaseTexture();

public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	bool Initialize(ID3D11Device*, const char*, const WCHAR*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();
};

