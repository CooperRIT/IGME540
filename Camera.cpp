#include "Camera.h"
using namespace DirectX;

Camera::Camera(DirectX::XMFLOAT3 pos, float moveSpeed, float lookSpeed, float fov, float aspectRatio)
{
	movementSpeed = moveSpeed;
	mouseLookSpeed = lookSpeed;
	fieldOfView = fov;

	transform = std::make_shared<Transform>();

	UpdateViewMatrix();

	UpdateProjectionMatrix(aspectRatio);
}

Camera::~Camera()
{
}

void Camera::Update(float deltaTime)
{

}

void Camera::UpdateViewMatrix()
{
	XMFLOAT3 pos = transform->GetPosition();
	XMFLOAT3 forward = transform->GetUp();
	XMFLOAT3 worldUp = XMFLOAT3(0, 1, 0);

	XMMATRIX viewM = XMMatrixLookToLH(
		XMLoadFloat3(&pos),
		XMLoadFloat3(&forward),
		XMLoadFloat3(&worldUp)
	);
	XMStoreFloat4x4(&viewMatrix, viewM);
}

void Camera::UpdateProjectionMatrix(float aspectRatio)
{
	//Call the DXMath function to make the perspective proj
	XMMATRIX proj = XMMatrixPerspectiveFovLH
	(
		XM_PIDIV4, //FOV anle in radians
		aspectRatio,
		.01f,
		1000.0f
	);
	XMStoreFloat4x4(&projMatrix, proj);


}

DirectX::XMFLOAT4X4 Camera::GetView()
{
	return viewMatrix;
}

DirectX::XMFLOAT4X4 Camera::GetProjection()
{
	return projMatrix;
}

std::shared_ptr<Transform> Camera::GetTransform()
{
	return transform;
}
