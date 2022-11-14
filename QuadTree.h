#pragma once
#include "Model.h"
#include "Frustum.h"
#include "Vertex.h"

#define MAX_MODELS_PER_QUAD 2 // How many models in each box that can be held in the quad tree

struct Node
{
	float positionX, positionZ, width;
	int modelCount;
	std::vector<int> modelIndex;
	std::vector<Model*> modelVector;
	Node* nodes[4];
};


// The QuadTreeClass will require a list of the vertices from the model class object 
// for building the quad tree. The list will be stored in the following array.
class QuadTree
{
public:
	QuadTree();
	~QuadTree();

	bool Initialize(std::vector <Model*>models);
	void Render(Frustum* frustum, ID3D11Device* device, ID3D11DeviceContext* context);
	void Shutdown();

private:
	Vertex vertexMember;
	Node nodeMember;

	void CreateTreeNode(Node* node, float positionX, float positionZ, float width);
	int CountModels(float positionX, float positionZ, float width);
	bool IsModelContained(int index, float positionX, float positionZ, float width);
	void ReleaseNode(Node* node);
	void RenderNode(Node* node, Frustum* frustum, ID3D11Device* device, ID3D11DeviceContext* context);

	std::vector<Vertex> vertexList;
	std::vector<Model*> modelList; // contains all models to be part of culling 
	std::vector<bool> modelIsRendered; // To keep models from being rendered twice

	int modelCount;
	float centerX, centerZ, width;

	// The parent node is the root of the quad tree. 
	// This single node will be expanded recursively to build the entire tree.
	Node* parentNode;
};


