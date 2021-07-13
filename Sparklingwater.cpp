#include "Sparklingwater.h"
#include "WICTextureLoader.h"


Water::Water()
{

}


Water::~Water()
{
	//release texture
	waterSRV->Release();

	//Release vertex and index buffer
	// Release the index buffer.
	if (iBuffer)
	{
		iBuffer->Release();
		iBuffer = 0;
	}

	// Release the vertex buffer.
	if (vBuffer)
	{
		vBuffer->Release();
		vBuffer = 0;
	}

	return;
}