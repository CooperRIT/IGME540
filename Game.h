#pragma once

#include "Mesh.h"
#include <memory>
#include <DirectXMath.h>
#include "Transform.h"

#include <d3d11.h>
#include <wrl/client.h>


struct VertexShaderToCopyToGpuToGPU
{
	DirectX::XMFLOAT4 colorTint;
	DirectX::XMFLOAT4X4 offset;
};

class Game
{
public:
	// Basic OOP setup
	Game() = default;
	~Game();
	Game(const Game&) = delete; // Remove copy constructor
	Game& operator=(const Game&) = delete; // Remove copy-assignment operator

	// Primary functions
	void Initialize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);
	void OnResize();

private:

	// Initialization helper methods - feel free to customize, combine, remove, etc.
	void LoadShaders();

	Transform transform;

	//Gui Helper Methods
	void UpdateImGui(float deltaTime);
	void BuildUI();
	void ChangeColor(float* _color, float r, float g, float b, float a);
	void CopyXMFloatToArray(DirectX::XMFLOAT3 xmFloat, float* floatArray);

	// Shaders and shader-related constructs
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

	//Gui Variables
	int currentSliderValue;
	int maxSliderValue = 100;
	int minSliderValue = 0;

	bool demoWindowState = false;

	float color[4];

	float objectColorTint[4];
	float objectOffset[3];

	//Mesh List
	Mesh* meshes;

	//constantbuffer
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;

};

