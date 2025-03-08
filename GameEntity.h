#pragma once

#include "Mesh.h";
#include "Transform.h";
#include <memory>;
#include "Camera.h"
#include "Material.h"

#include <d3d11.h>
#include <wrl/client.h>

class GameEntity
{
	public:
		GameEntity(Mesh _mesh, Material mat);
		~GameEntity();

		std::shared_ptr<Transform> GetTransform();
		std::shared_ptr<Mesh> GetMesh();
		std::shared_ptr<Material> GetMaterial();

		void Draw(std::shared_ptr<Camera> camera, float time);


	private:
		std::shared_ptr<Mesh> mesh;
		std::shared_ptr<Transform> transform;
		std::shared_ptr<Material> material;

		Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;
};

