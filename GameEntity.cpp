#include "GameEntity.h"

#include <d3d11.h>
#include <wrl/client.h>
#include "Game.h";


GameEntity::GameEntity(Microsoft::WRL::ComPtr<ID3D11Buffer>& _constantBuffer, Mesh _mesh)
{
	//Assign constant buffer
	constantBuffer = _constantBuffer;

	//material = std::make_shared<Material>(mat);

	//Create Mesh
	mesh = std::make_shared<Mesh>(_mesh);

	//Create Transform
	transform = std::make_shared<Transform>();

}

GameEntity::~GameEntity()
{

}

std::shared_ptr<Transform> GameEntity::GetTransform()
{
	return transform;
}

std::shared_ptr<Mesh> GameEntity::GetMesh()
{
	return mesh;
}

std::shared_ptr<Material> GameEntity::GetMaterial()
{
	return material;
}

void GameEntity::Draw(std::shared_ptr<Camera> camera)
{
	//Constant Buffer Mapping
	VertexShaderToCopyToGpuToGPU dataToCopy{};
	dataToCopy.worldMatrix = transform->GetWorldMatrix();
	dataToCopy.viewMatrix = camera->GetView();
	dataToCopy.projectionMatrix = camera->GetProjection();


	//First we need to Map the buffer
	D3D11_MAPPED_SUBRESOURCE mapped{};
	Graphics::Context->Map(
		constantBuffer.Get(),
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mapped);

	//Copy To GPU using memcpy
	memcpy(mapped.pData, &dataToCopy, sizeof(VertexShaderToCopyToGpuToGPU));

	//Unmap when done
	Graphics::Context->Unmap(constantBuffer.Get(), 0);


	mesh->Draw();
}


