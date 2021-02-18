#include "orthowindowclass.h"

OrthoWindowClass::OrthoWindowClass()
{
	vertexBuffer = 0;
	indexBuffer = 0;
}


OrthoWindowClass::OrthoWindowClass(const OrthoWindowClass& other)
{
}


OrthoWindowClass::~OrthoWindowClass()
{
}

bool OrthoWindowClass::Initialize(ID3D11Device* device, int windowWidth, int windowHeight)
{
	bool hr = false;

	hr = InitializeBuffers(device, windowWidth, windowHeight);
	if(!hr)
	{
		return false;
	}

	return true;
}



void OrthoWindowClass::Render(ID3D11DeviceContext* immediateContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(immediateContext);

	return;
}


int OrthoWindowClass::GetIndexCount()
{
	return indexCount;
}

bool OrthoWindowClass::InitializeBuffers(ID3D11Device* device, int windowWidth, int windowHeight)
{
	float left, right, top, bottom = 0.0f;
	VertexType* vertices = nullptr;
	unsigned long* indices = nullptr;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT hr;
	


	// Calculate coordinates of left side of window.
	left = (float)((windowWidth / 2) * -1);
	
	// Calculate coordinates of right side of  window.
	right = left + (float)windowWidth;

	// Calculate coordinates of top of window.
	top = (float)(windowHeight / 2);

	// Calculate screen coordinates of bottom of window.
	bottom = top - (float)windowHeight;

	
	vertexCount = 6;

	indexCount = vertexCount;


	
	vertices = new VertexType[vertexCount];
	if(!vertices)
	{
		std::cout << "problem with vertices in orthoWindowClass" << std::endl;
		return false;
	}


	indices = new unsigned long[indexCount];
	if(!indices)
	{
		std::cout << "problem with indices in orthoWindowClass" << std::endl;
		return false;
	}

	// Load the vertex array with data.
	// First triangle.
	vertices[0].position = dx::XMFLOAT3(left, top, 0.0f);  // Top left.
	vertices[0].texture = dx::XMFLOAT2(0.0f, 0.0f);

	vertices[1].position = dx::XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
	vertices[1].texture = dx::XMFLOAT2(1.0f, 1.0f);

	vertices[2].position = dx::XMFLOAT3(left, bottom, 0.0f);  // Bottom left.
	vertices[2].texture = dx::XMFLOAT2(0.0f, 1.0f);

	// Second triangle.
	vertices[3].position = dx::XMFLOAT3(left, top, 0.0f);  // Top left.
	vertices[3].texture = dx::XMFLOAT2(0.0f, 0.0f);

	vertices[4].position = dx::XMFLOAT3(right, top, 0.0f);  // Top right.
	vertices[4].texture = dx::XMFLOAT2(1.0f, 0.0f);

	vertices[5].position = dx::XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
	vertices[5].texture = dx::XMFLOAT2(1.0f, 1.0f);

	// Load the index array with data.
	for(int i = 0; i < indexCount; i++)
	{
		indices[i] = i;
	}


	// Set up the description of the vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	// Give subresource structure a pointer to vertex data.
	vertexData.pSysMem = vertices;

	// Now finally create the vertex buffer.
	hr = device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
	if(FAILED(hr))
	{
		std::cerr << "ERROR creating vertex buffer in orthoWindowClass!" << std::endl;
		return false;
	}

	// Set desc of the index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;

	// Create the index buffer.
	hr = device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);
	if(FAILED(hr))
	{
		std::cerr << "Error creating index buffer in orthoWindowClass" << std::endl;
		return false;
	}


	// Release stuff
	delete [] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;

	return true;
}


void OrthoWindowClass::ShutdownBuffers()
{
	// Release 
	if (indexBuffer)
	{
		indexBuffer->Release();
		indexBuffer = 0;
	}

	// Release
	if (vertexBuffer)
	{
		vertexBuffer->Release();
		vertexBuffer = 0;
	}

	return;
}

void OrthoWindowClass::RenderBuffers(ID3D11DeviceContext* immediateContext)
{
	unsigned int stride, offset = 0;

	stride = sizeof(VertexType); 

    
	// Set the vertex buffer to active in the input assembler so it can be rendered.
	immediateContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	
	// Set the index buffer to active in the input assembler so it can be rendered.
	immediateContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}