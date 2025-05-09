#pragma once
#include "Graphics.h"
#include "Vertex.h"

#include <fstream>
#include <stdexcept>
#include <vector>

#include <d3d11.h>
#include <wrl/client.h>

class Mesh
{
public:
	Mesh(Vertex* vertices, int vertexCount, unsigned int* indicies, int indexCount);
	Mesh(const char* objFile);
	~Mesh();

	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();

	int GetIndexCount();
	int GetVertexCount();

	void Draw();
	
	DirectX::XMFLOAT4 XMGetColor();
	float* PtrGetColor();
	
	//const char* name;
private:

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

	DirectX::XMFLOAT4 color;

	int indexBufferCount;
	int vertexBufferCount;

	//Methods
	void CreateMeshBuffers(Vertex* vertices, int vertexCount, unsigned int* indices, int indexCount);

	void CalculateTangents(Vertex* verts, int numVerts, unsigned int* indices, int numIndices);
};