#include "Transform.h"

using namespace DirectX;

Transform::Transform() : position(0, 0, 0), pitchYawRoll(0, 0, 0), scale(1, 1, 1), dirty(false)
{
	XMStoreFloat4x4(&worldMatrix, XMMatrixIdentity());

	up = XMFLOAT3(0, 1, 0);
	right = XMFLOAT3(1, 0, 0);
	forward = XMFLOAT3(0, 0, 1);

}

void Transform::SetPosition(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;
	dirty = true;
}

void Transform::SetPosition(DirectX::XMFLOAT3 pos)
{
	position = pos;
	dirty = true;
}

void Transform::SetPosition(float* array)
{
	SetPosition(array[0], array[1], array[2]);
}


void Transform::SetRotation(float ptich, float yaw, float roll)
{
	pitchYawRoll.x = ptich;
	pitchYawRoll.y = yaw;
	pitchYawRoll.z = roll;
	dirty = true;
	UpdateVectors();
}

void Transform::SetRotation(DirectX::XMFLOAT3 rotation)
{
	pitchYawRoll = rotation;
}

void Transform::SetRotation(float* array)
{
	SetRotation(array[0], array[1], array[2]);
}

void Transform::SetScale(float x, float y, float z)
{
	scale.x = x;
	scale.y = y;
	scale.z = z;
	dirty = true;

}

void Transform::SetScale(DirectX::XMFLOAT3 _scale)
{
	scale = _scale;
}

void Transform::SetScale(float* array)
{
	SetScale(array[0], array[1], array[2]);
}

void Transform::MoveAbsolute(float x, float y, float z)
{
	XMStoreFloat3(&position, XMLoadFloat3(&position) + XMVectorSet(x, y, z, 0));
}

void Transform::MoveAbsolute(DirectX::XMFLOAT3 offset)
{

}

void Transform::MoveRelative(float x, float y, float z)
{
	//Move along our "local" axes
	
	// Step 1: Create a vector
	XMVECTOR movment = XMVectorSet(x, y, z, 0);

	//Createa a quaterninon for rotation

	XMVECTOR rotQuat = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&pitchYawRoll));

	//Perform the rotation of our desired movement
	XMVECTOR dir = XMVector3Rotate(movment, rotQuat);

	//Step 4: Add this rotated direction to our position
	XMStoreFloat3(&position, XMLoadFloat3(&position) + dir);
	dirty = true;
	UpdateVectors();
}

void Transform::Rotate(float ptich, float yaw, float roll)
{
	pitchYawRoll.x += ptich;
	pitchYawRoll.y += yaw;
	pitchYawRoll.z += roll;

	UpdateVectors();
}

void Transform::Scale(float x, float y, float z)
{
	scale.x *= x;
	scale.y *= y;
	scale.z *= z;
}

DirectX::XMFLOAT3 Transform::GetPosition()
{
	return position;
}

DirectX::XMFLOAT3 Transform::GetRotation()
{
	return pitchYawRoll;
}

DirectX::XMFLOAT3 Transform::GetScale()
{
	return scale;
}

DirectX::XMFLOAT3 Transform::GetUp()
{
	return up;
}
DirectX::XMFLOAT3 Transform::GetForward()
{
	return forward;
}

DirectX::XMFLOAT4X4 Transform::GetWorldMatrix()
{
	//Guard clause is cleaner :)
	if (!dirty)
	{
		return worldMatrix;
	}

	//Make translation, rotation, and scale matricies

	XMMATRIX trMatrix = XMMatrixTranslationFromVector(XMLoadFloat3(&position));

	XMMATRIX rotMatrix = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&pitchYawRoll));

	XMMATRIX scMatrix = XMMatrixScalingFromVector(XMLoadFloat3(&scale));



	//World Matrix is a combo of tranlateion rotation and scale matricies

	XMMATRIX _worldMatrix = scMatrix * rotMatrix * trMatrix;

	XMStoreFloat4x4(&worldMatrix, _worldMatrix);

	dirty = false;

	UpdateVectors();

	return worldMatrix;
}

DirectX::XMFLOAT4X4 Transform::GetInverseTransposeMatrix()
{
	DirectX::XMMATRIX matrix = DirectX::XMLoadFloat4x4(&worldMatrix);

	// Inverse-transpose: First, invert the matrix, then transpose it
	DirectX::XMMATRIX inverseTranspose = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, matrix));

	// Store the result back into a float4x4
	DirectX::XMFLOAT4X4 result;
	DirectX::XMStoreFloat4x4(&result, inverseTranspose);

	return result;
}

void Transform::UpdateVectors()
{
	// Create a rotation matrix from the pitchYawRoll angles
	XMMATRIX rotMatrix = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&pitchYawRoll));

	// Transform the initial forward, up, and right vectors using the rotation matrix
	XMStoreFloat3(&forward, XMVector3TransformNormal(XMVectorSet(0, 0, 1, 0), rotMatrix));
	XMStoreFloat3(&up, XMVector3TransformNormal(XMVectorSet(0, 1, 0, 0), rotMatrix));
	XMStoreFloat3(&right, XMVector3TransformNormal(XMVectorSet(1, 0, 0, 0), rotMatrix));

	// Normalize the vectors to ensure they are unit vectors
	XMStoreFloat3(&forward, XMVector3Normalize(XMLoadFloat3(&forward)));
	XMStoreFloat3(&up, XMVector3Normalize(XMLoadFloat3(&up)));
	XMStoreFloat3(&right, XMVector3Normalize(XMLoadFloat3(&right)));
}
