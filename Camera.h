#pragma once

#include "Transform.h"
#include <DirectXMath.h>

#include<memory>

class Camera
{

public:
	Camera(DirectX::XMFLOAT3 pos, float moveSpeed, float lookSpeed, float fov, float aspectRatio);
	~Camera();

	//Update Methods
	void Update(float deltaTime);
	void UpdateViewMatrix();
	void UpdateProjectionMatrix(float aspectRatio);

	//Getters
	DirectX::XMFLOAT4X4 GetView();
	DirectX::XMFLOAT4X4 GetProjection();
	std::shared_ptr <Transform> GetTransform();

private:
	//camera matricies
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projMatrix;

	std::shared_ptr<Transform> transform;


	float fieldOfView;
	float movementSpeed;
	float mouseLookSpeed;

};

