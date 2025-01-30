#pragma once
#include "Graphics.h"
#include "Vertex.h"

#include <d3d11.h>
#include <wrl/client.h>

class Mesh
{
public:
	Mesh(Vertex* vertices, int vertexCount, unsigned int* indicies, int indexCount);
	~Mesh();

	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();

	int GetIndexCount();
	int GetVertexCount();

	void Draw();
	

private:

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

	int indexBufferCount;
	int vertexBufferCount;
};