#include "Defines.h"
#include "Mesh.h"


Mesh::Mesh(ID3D11Device* device, std::string name)
	:name(name)
{
	if (!LoadModel(name))
	{
		std::cout << "ERROR! COULD NOT LOAD MODEL DATA" << std::endl;
	}
	if (!LoadMaterial(device, name))
	{
		std::cout << "ERRO! COULD NOT LOAD MATERIAL DATA" << std::endl;
	}
}

Mesh::~Mesh()
{
	
}

ID3D11Buffer* Mesh::GetMTLBuffer() 
{ 
	return this->mtlBuffer; 
}

// OBJ reader
bool Mesh::LoadModel(std::string name)
{
	std::vector<std::array<float, 3>> vertex; // v is a vertex
	std::vector<std::array<float, 2>> vertexTexture; // vt is the texture coordinate of one vertex
	std::vector<std::array<float, 3>> vertexNormal;
	std::vector<std::array<std::array<int, 3>, 3>> f; // face containing 3 vertices

	std::ifstream reader;

	reader.open("Models/" + name + "/" + name + ".obj", std::ios::beg); // Open the .obj-file

	if (!reader.is_open())
		return false;

	std::string line;
	while (std::getline(reader, line))
	{
		std::string text;
		reader >> text;
		if (text == "v") 
		{
			std::array<float, 3> vertice;

			reader >> vertice[0];	// load the first float to index 0
			reader >> vertice[1];	// load the second float to index 1
			reader >> vertice[2];	// load the thrid float to index 2

			vertex.push_back(vertice);
		}

		if (text == "vt") 
		{
			std::array<float, 2> texCoord;

			reader >> texCoord[0]; 	  // load first float
			reader >> texCoord[1];	  // load second float
			texCoord[1] = 1 - texCoord[1];	// inverting since directX reads it upsidedown if I don't
			vertexTexture.push_back(texCoord);
		}

		if (text == "vn") // Normal of one vertex
		{
			std::array<float, 3> normal;
			reader >> normal[0];
			reader >> normal[1];
			reader >> normal[2];
			vertexNormal.push_back(normal);
		}

		if (text == "f") // Face with 3 verties
		{
			std::array<std::array<int, 3>, 3> face;

			std::string tempString;

			for (int i = 0; i < 3; ++i)
			{
				reader >> tempString;

				std::string substr;

				// in an obj:
				// f 1/2/3 4/5/6 7/8/9
				//    v0	v1	  v2     position, texcoord(uv), normal

				face[0][i] = std::stoi(tempString.substr(0, tempString.find("/"))); // position 

				substr = tempString.substr(tempString.find("/") + 1, tempString.length());
				face[1][i] = std::stoi(substr.substr(0, substr.find("/")));	// texcoord
				face[2][i] = std::stoi(substr.substr(substr.find("/") + 1, substr.length()));//normal
			}

			f.push_back(face);
		}
	}


	for (int i = 0; i < f.size(); ++i)
	{
		std::array<float, 3> position;
		std::array<float, 2> tex;
		std::array<float, 3> normal;
		std::array<Vertex, 3> vertices;

		for (int j = 0; j < 3; ++j)
		{
			//position of vertex j
			int index = f[i][0][j] - 1;
			position[0] = vertex[index][0];
			position[1] = vertex[index][1];
			position[2] = vertex[index][2];

			//texCoord of vertex j
			index = f[i][1][j] - 1;
			tex[0] = vertexTexture[index][0];
			tex[1] = vertexTexture[index][1];

			//normal of vertex j
			index = f[i][2][j] - 1;
			normal[0] = vertexNormal[index][0];
			normal[1] = vertexNormal[index][1];
			normal[2] = vertexNormal[index][2];

			positions.push_back({ position[0],position[1],position[2] });//used for shadows
			vertices[j] = Vertex(position, tex, normal);
			vertexCount++;
		}

		Face face = Face(vertices[0], vertices[1], vertices[2]);
		faces.push_back(face);
	}
	return true;
}

// Material Reader
bool Mesh::LoadMaterial(ID3D11Device* device, std::string name)
{
	std::ifstream reader;
	reader.open("Models/" + name + "/" + name + ".mtl", std::ios::beg);	// opens MTL-file
	if (!reader.is_open())
	{
		return false;
	}
	int counter = 0;
	std::string line;
	while (std::getline(reader, line))
	{
		std::string text;
		reader >> text;
		if (text == "Kd")//this is diffuse, gets stored in the struct
		{
			XMFLOAT4 diffuse;

			reader >> diffuse.x;
			reader >> diffuse.y;
			reader >> diffuse.z;
			diffuse.w = 0;
			material.data.diffuse = diffuse;
		}

		if (text == "Ka")	//this is ambient, gets stored in struct
		{
			XMFLOAT4 ambient;

			reader >> ambient.x;
			reader >> ambient.y;
			reader >> ambient.z;
			ambient.w = 0;

			material.data.ambient = ambient;
		}

		if (text == "Ks") //this is specular, gets stored in struct
		{
			XMFLOAT4 specular;

			reader >> specular.x;
			reader >> specular.y;
			reader >> specular.z;
			specular.w = 0;

			material.data.specular = specular;
		}

		if (text == "map_Kd") //this is texture which is used, gets stored in material vector		
		{  //this can be changed in the mtl file
			std::string diffuse;
			reader >> diffuse;
			material.diffuseTextures.emplace_back(new Texture(device, "Models/" + name + "/" + diffuse));
		}

	}

	// buffer per model containing material data
	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = sizeof(material.data);
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	HRESULT hr = device->CreateBuffer(&desc, nullptr, &mtlBuffer);
	if FAILED(hr)
	{
		std::cerr << "ERROR! COULD NOT CREATE MTL BUFFER" << std::endl;
	}
	return true;
}

void Mesh::Shutdown()
{
	mtlBuffer->Release();
}

