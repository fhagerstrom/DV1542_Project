#include "QuadTree.h"

QuadTree::QuadTree()
{
}

QuadTree::~QuadTree()
{
}

bool QuadTree::Initialize(std::vector<Model*> models)
{
	int vertexCount = 0;
	modelCount = 0;
	centerX = 0;
	centerZ = 0;
	width = 50.0f;

	// Loop through all models in model vector...
	for (auto& model : models)
	{
		// If it is the city (large area) or the triforce (moving object)
		if (models.at(0) || models.at(4)) // index 0 = city, index 4 = triforce 
		{
			// We are not doing anything with moving/rotating objects or the plane / city.
		}
		else
		{
			modelCount += 1;
			modelList.push_back(model);
			modelIsRendered.push_back(false);
		}
	}

	// Create the parent node for the quad tree.
	parentNode = new Node;
	if (!parentNode)
	{
		MessageBox(NULL, L"Couldn't create the parent node for the frustum!", L"FRUSTUM ERROR", MB_OK);
		return false;
	}

	CreateTreeNode(parentNode, centerX, centerZ, width);

	return true;
}

void QuadTree::Render(Frustum* frustum, ID3D11Device* device, ID3D11DeviceContext* context)
{
	// Render each node that is visible starting at the parent node and moving down the tree.
	// RenderNode(parentNode, frustum, device, context);
	int i = 0;
	for (bool culledModels : modelIsRendered)
	{
		modelIsRendered[i] = false;
		i++;
	}
}

void QuadTree::Shutdown()
{
	// Release the quad tree data.
	if (parentNode)
	{
		// ReleaseNode(parentNode);
		delete parentNode;
		parentNode = 0;
	}

	// Clear vectors
	modelList.clear();
	modelIsRendered.clear();
}

void QuadTree::CreateTreeNode(Node* node, float positionX, float positionZ, float width) // origo of quad and width
{
	int numModels = 0, count = 0, vertexCount = 0, index = 0, vertexIndex = 0;
	float offsetX = 0.0f, offsetZ = 0.0f;
	bool result;

	// First initialize the node and set its position in the world.
	// Store the node position and size.
	node->positionX = positionX;
	node->positionZ = positionZ;
	node->width = width;

	// Initialize the model count to zero for the node
	node->modelCount = 0;

	// Initialize the children nodes of this node to zero as well
	node->nodes[0] = 0;
	node->nodes[1] = 0;
	node->nodes[2] = 0;
	node->nodes[3] = 0;

	// Count the number of models that are inside this node.
	numModels = CountModels(positionX, positionZ, width);


	// Case 1: If there are no models in this node then return as it is empty and requires no processing.
	if (numModels == 0)
	{
		return;
	}

	// Case 2: If there are too many models in this node then split it into four equal sized smaller tree nodes.
	if (numModels > MAX_MODELS_PER_QUAD)
	{
		for (int i = 0; i < 4; i++)
		{
			// Calculate the position offsets for the new child nodes
			if (i == 0)
			{
				offsetX = width / 4.0f;
				offsetZ = width / 4.0f;
			}
			else if (i == 1)
			{
				offsetX = -width / 4.0f;
				offsetZ = width / 4.0f;
			}
			else if (i == 2)
			{
				offsetX = width / 4.0f;
				offsetZ = -width / 4.0f;
			}
			else
			{
				offsetX = -width / 4.0f;
				offsetZ = -width / 4.0f;
			}

			// See if there are any triangles in the new node.
			// count = CountModels((positionX + offsetX), (positionZ + offsetZ), (width / 2.0f));
			if (count > 0)
			{
				// If there are models inside where this new node would be then create the child node.
				node->nodes[i] = new Node;

				// Extend the tree starting from this new child node now.
				CreateTreeNode(node->nodes[i], (positionX + offsetX), (positionZ + offsetZ), (width / 2.0f));
			}
		}
	}

	// Case 3: If this node is not empty and the triangle count for it is equal to or lower than the max then 
	// this node is at the bottom of the tree so we create the list of triangles to store in it.
	node->modelCount = numModels;
	// Initialize the index for this new vertex and index array.
	index = 0;

	// Go through all the models in the models list.
	for (int i = 0; i < modelCount; i++)
	{
		// If the model is inside this node then add it to the model vector.
		result = IsModelContained(i, positionX, positionZ, width);
		if (result == true)
		{
			node->modelVector.push_back(modelList[i]);
			node->modelIndex.push_back(i);
		}
	}
}

int QuadTree::CountModels(float positionX, float positionZ, float width)
{
	int count = 0;

	// Go through all the models in the entire vector and check which ones should be inside this node.
	for (int i = 0; i < modelCount; i++)
	{
		// If the triangle is inside the node then increment the count by one.
		bool result = IsModelContained(i, positionX, positionZ, width);
		if (result == true)
		{
			count++;
		}
	}
	return count;
}

bool QuadTree::IsModelContained(int index, float positionX, float positionZ, float width)
{
	float xCenter = 0.0f, zCenter = 0.0f;

	float radius = width / 2.0f;

	// Get the index into the vertex list.
	Model* tempModel = modelList[index];

	// xCenter = tempModel->posForQuads.x; // Model coords. DOUBLE CHECK
	// zCenter = tempModel->posForQuads.z; 


	if (xCenter <= positionX + radius)
	{
		if (xCenter >= positionX - radius)
		{
			if (zCenter <= positionZ + radius)
			{
				if (zCenter >= positionZ - radius)
				{
					return true;
				}
			}
		}
	}
	return false;
}

void QuadTree::ReleaseNode(Node* node)
{
	// Recursively go down the tree and release the bottom nodes first.
	for (int i = 0; i < 4; i++)
	{
		if (node->nodes[i] != 0)
		{
			ReleaseNode(node->nodes[i]);
		}
	}
	node->modelVector.clear();

	// Release the four child nodes.
	for (int i = 0; i < 4; i++)
	{
		if (node->nodes[i])
		{
			node->modelVector.clear();
			node->modelIndex.clear();
			delete node->nodes[i];
			node->nodes[i] = 0;
		}
	}
	return;
}

void QuadTree::RenderNode(Node* node, Frustum* frustum, ID3D11Device* device, ID3D11DeviceContext* context)
{
	int count = 0;

	// Check to see if the node can be viewed, height doesn't matter in a quad tree.
	bool result = frustum->CheckCube(node->positionX, 0.0f, node->positionZ, (node->width / 2.0f));

	// If it can't be seen then none of its children can either so don't continue down the tree, this is where the speed is gained.
	if (!result)
	{
		return;
	}

	// If it can be seen then check all four child nodes to see if they can also be seen.
	count = 0;
	for (int i = 0; i < 4; i++)
	{
		if (node->nodes[i] != 0)
		{
			count++;
			RenderNode(node->nodes[i], frustum, device, context);
		}
	}

	// If there were any children nodes then there is no need to continue as parent nodes won't contain any triangles to render.
	if (count != 0)
	{
		return;
	}
	// Otherwise if this node can be seen and has models in it then render these models.
	for (int i = 0; i < node->modelCount; i++)
	{
		if (modelIsRendered[node->modelIndex[i]] == false) //see if it is already rendered, if not, render it
		{	// modelIndex contains correct index for model(s)
			modelIsRendered[node->modelIndex[i]] = true;
			// renderer->Render(device, context, modelList[node->modelIndex[i]], false);
		}
		//else:			node->modelVector[i]
		//if it is already rendered, we dont need to render it again.
	}
}