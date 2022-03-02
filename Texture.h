#include <d3d11.h>
#include <WICTextureLoader.h>

class Texture
{
private:
	HRESULT hr;
	ID3D11Texture2D* texture;
	ID3D11ShaderResourceView* textureSrv;

public:
	Texture();
	Texture(const Texture&);
	~Texture();

	bool Initialize(ID3D11Device* device, LPCWSTR textureFile);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture() { return textureSrv; };

};