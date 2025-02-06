#include "Transform.h"

using namespace DirectX;

Transform::Transform() : position(0, 0, 0), pitchYawRoll(0, 0, 0), scale(1, 1, 1), dirty(false)
{
	XMStoreFloat4x4(&worldMatrix, XMMatrixIdentity());

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
}



void Transform::SetRotation(float ptich, float yaw, float roll)
{
	pitchYawRoll.x = ptich;
	pitchYawRoll.y = yaw;
	pitchYawRoll.z = roll;
	dirty = true;

}

void Transform::SetScale(float x, float y, float z)
{
	scale.x = x;
	scale.y = y;
	scale.z = z;
	dirty = true;

}

void Transform::MoveAbsolute(float x, float y, float z)
{
	XMStoreFloat3(&position, XMLoadFloat3(&position) + XMVectorSet(x, y, z, 0));
}

void Transform::Rotate(float ptich, float yaw, float roll)
{
	pitchYawRoll.x += ptich;
	pitchYawRoll.y += yaw;
	pitchYawRoll.z += roll;
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

	return worldMatrix;
}
