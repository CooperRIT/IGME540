#pragma once

#include <DirectXMath.h>

//Header files are copy and pasted into cpp files at run time
class Transform
{
public:
	Transform();

	//Setters
	void SetPosition(float x, float y, float z);
	void SetPosition(DirectX::XMFLOAT3 pos);
	void SetPosition(float* array);
	void SetRotation(float ptich, float yaw, float roll);
	void SetRotation(DirectX::XMFLOAT3 rotation);
	void SetRotation(float* array);
	void SetScale(float x, float y, float z);
	void SetScale(DirectX::XMFLOAT3 _scale);
	void SetScale(float* array);

	//Transformers
	void MoveAbsolute(float x, float y, float z);
	void MoveAbsolute(DirectX::XMFLOAT3 offset);

	void MoveRelative(float x, float y, float z);

	void Rotate(float ptich, float yaw, float roll);
	void Scale(float x, float y, float z);

	//Getters
	DirectX::XMFLOAT3 GetPosition();
	DirectX::XMFLOAT3 GetRotation();
	DirectX::XMFLOAT3 GetScale();

	DirectX::XMFLOAT3 GetUp();
	DirectX::XMFLOAT3 GetForward();


	DirectX::XMFLOAT4X4 GetWorldMatrix();


private:

	//Transform data
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 pitchYawRoll; //Rotation
	DirectX::XMFLOAT3 scale;
	// Quaternion Version For Later DirectX::XMFLOAT4 rotation;

	//localOrentation
	DirectX::XMFLOAT3 up;
	DirectX::XMFLOAT3 right;
	DirectX::XMFLOAT3 forward;

	//Matrix
	bool dirty;
	DirectX::XMFLOAT4X4 worldMatrix;

	void UpdateVectors();
	bool vectorDirty = false;
};

