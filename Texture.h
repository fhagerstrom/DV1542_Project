#include <d3d11.h>
#include <WICTextureLoader.h>

#include <string>

class Texture
{
private:
	HRESULT hr;
	// ID3D11Texture2D* textureSRV;
	ID3D11ShaderResourceView* textureSrv;
	std::string name;

public:
	Texture();
	~Texture();

	bool Initialize(ID3D11Device* device, LPCWSTR textureFile);
	void Shutdown();

	void SetTexture(ID3D11ShaderResourceView* resource) { this->textureSrv = resource; }
	ID3D11ShaderResourceView* GetTexture() { return textureSrv; }

	void SetName(std::string name) { this->name = name; }
	std::string GetName() { return this->name; }

};