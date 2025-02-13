#pragma once

#include "Mesh.h";
#include "Transform.h";
#include <memory>;
#include "Camera.h"

#include <d3d11.h>
#include <wrl/client.h>

class GameEntity
{
	private:
		std::shared_ptr<Mesh> mesh;
		std::shared_ptr<Transform> transform;

		Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;

	public:
		GameEntity(Microsoft::WRL::ComPtr<ID3D11Buffer>& _constantBuffer,Mesh _mesh);
		~GameEntity();

		std::shared_ptr<Transform> GetTransform();
		std::shared_ptr<Mesh> GetMesh();

		void Draw(DirectX::XMFLOAT4 colorTint, std::shared_ptr<Camera> camera);

};

