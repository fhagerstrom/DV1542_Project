#pragma once

class Shadow
{
private:
	ID3D11ShaderResourceView* depthMapSRV;	
	ID3D11DepthStencilView* depthMapDSV;
	D3D11_VIEWPORT viewPort;
public:
	Shadow() = default;
	Shadow(ID3D11Device* device, UINT size); 
	~Shadow();

	void ShutDown();
	ID3D11ShaderResourceView** GetSRV();

	// Function to bind the shadowmap
	void Bind(ID3D11DeviceContext* context);
};