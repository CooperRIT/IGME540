#include "GameEntity.h"

#include <d3d11.h>
#include <wrl/client.h>
#include "Game.h";


GameEntity::GameEntity(Mesh _mesh, Material mat)
{
	//Create Mesh
	mesh = std::make_shared<Mesh>(_mesh);

	//Create Material
	material = std::make_shared<Material>(mat);

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
	material->GetPS()->SetShader();
	material->GetVS()->SetShader();

	std::shared_ptr<SimpleVertexShader> vs = material->GetVS();
	vs->SetFloat4("colorTint", material->GetColorTint()); // Strings here MUST
	vs->SetMatrix4x4("world", transform->GetWorldMatrix()); // match variable
	vs->SetMatrix4x4("view", camera->GetView()); // names in your
	vs->SetMatrix4x4("projection", camera->GetProjection()); // shader’s cbuffer!

	vs->CopyAllBufferData();

	mesh->Draw();
}


