#include "Camera.h"
#include "Input.h"

using namespace DirectX;

Camera::Camera(DirectX::XMFLOAT3 pos, float moveSpeed, float lookSpeed, float fov, float aspectRatio)
{
	movementSpeed = moveSpeed;
	mouseLookSpeed = lookSpeed;
	fieldOfView = fov;

	transform = std::make_shared<Transform>();

	transform->SetPosition(pos);

	UpdateViewMatrix();

	UpdateProjectionMatrix(aspectRatio);
}

Camera::~Camera()
{
}

void Camera::Update(float deltaTime)
{
	//W
	if (Input::KeyDown('W'))
	{
		transform->MoveRelative(0, 0, 1 * deltaTime);
	}

	//A
	if (Input::KeyDown('A'))
	{
		transform->MoveRelative(-1 * deltaTime, 0, 0);
	}

	//S
	if (Input::KeyDown('S'))
	{
		transform->MoveRelative(0, 0, -1 * deltaTime);
	}

	//D
	if (Input::KeyDown('D'))
	{
		transform->MoveRelative(1 * deltaTime, 0, 0);
	}

	//Mouse Movement
	if (Input::MouseLeftDown())
	{
		int cursorMovementX = Input::GetMouseXDelta() * mouseLookSpeed;
		int cursorMovementY = Input::GetMouseYDelta() * mouseLookSpeed;

		cursorMovementX = (int)(Clamp(cursorMovementX, -0.5f * DirectX::XM_PI, 0.5f * DirectX::XM_PI));


		printf("efojwejf");

		transform->Rotate(cursorMovementY * deltaTime, cursorMovementX * deltaTime, 0);
	}


	UpdateViewMatrix();
}

void Camera::UpdateViewMatrix()
{
	XMFLOAT3 pos = transform->GetPosition();
	XMFLOAT3 forward = transform->GetForward();
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

float Camera::Clamp(float input, float min, float max)
{
   if (input > max)
	{
		return max;
	}
	return input < min ? min : input;
}
