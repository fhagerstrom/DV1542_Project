#pragma once
#include "Model.h"
#include "Frustum.h"
#include "RenderModels.h"
#include "Vertex.h"

const int MAXMODELSPERQUAD = 2;

class QuadTree
{
private:
	struct VertexType
	{
		float position[3];
		float texture[2];
		float normal[3];
	};
	//Each node in the quad tree will be defined as follows with position, size, triangle count, buffers, and four child nodes :

	struct Node
	{
		float positionX, positionZ, width;
		int modelCount;
		std::vector<int> modelIndex; //for lists
		std::vector<Model*> modelVector;
		Node* nodes[4];
	};

public:
	QuadTree();
	QuadTree(const QuadTree&);
	~QuadTree();
	
	bool Initialize(std::vector <Model*>models);
	void Render(Frustum* frustum, ID3D11Device* device, ID3D11DeviceContext* context, RenderModels* renderer);
	void Shutdown();


private:
	//void CalculateMeshDimensions(Model* tempModel, float& centerX, float& centerZ, float& minimumX, float& maximumX, float& minimumZ, float& maximumZ);
	void CreateTreeNode(Node* node, float positionX, float positionZ, float width);
	int CountModels(float positionX, float positionZ, float width);
	bool IsModelContained(int index, float positionX, float positionZ, float width);
	void ReleaseNode(Node* node);
	void RenderNode(Node* node, Frustum* frustum, ID3D11Device* device, ID3D11DeviceContext* context, RenderModels* renderer);

	int modelCount; // For objects, not plane

	//The QuadTreeClass will require a list of the vertices from the model class object for building the quad tree.The list will be stored in the following array.
	std::vector<Vertex> vertexList;
	//VertexType* vertexList;
	//Vertex* vertexList;
	//VertexType* objectsList;
	std::vector<Model*> modelList; // contains all models to be part of culling 
	std::vector<bool> modelIsRendered; //to keep models from being rendered twice
	float centerX, centerZ, width;
	//minX, maxX, minZ, maxZ,
	//The parent node is the root of the quad tree. This single node will be expanded recursively to build the entire tree.
	Node* parentNode;
};