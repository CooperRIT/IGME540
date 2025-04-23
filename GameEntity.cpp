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

void GameEntity::Draw(std::shared_ptr<Camera> camera, float time, DirectX::XMFLOAT4X4 lightViewMatrix, DirectX::XMFLOAT4X4 lightProjectionMatrix)
{
	material->GetPS()->SetShader();
	material->GetVS()->SetShader();

	std::shared_ptr<SimpleVertexShader> vs = material->GetVS();
	vs->SetMatrix4x4("world", transform->GetWorldMatrix());
	vs->SetMatrix4x4("view", camera->GetView());
	vs->SetMatrix4x4("projection", camera->GetProjection());
	vs->SetMatrix4x4("worldInvTranspose", transform->GetInverseTransposeMatrix());
	vs->SetMatrix4x4("lightView", lightViewMatrix);
	vs->SetMatrix4x4("lightProjection", lightProjectionMatrix);


	std::shared_ptr<SimplePixelShader> ps = material->GetPS();
	ps->SetFloat4("colorTint", material->GetColorTint());
	ps->SetFloat2("uvScale", material->GetUVScale());
	ps->SetFloat2("uvOffset", material->GetUVOffset());
	ps->SetFloat("time", time);

	material->PrepareMaterial(camera->GetTransform()->GetPosition());

	vs->CopyAllBufferData();
	ps->CopyAllBufferData();

	mesh->Draw();
}


