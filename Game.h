#pragma once

#include "Mesh.h"
#include <memory>
#include <DirectXMath.h>

#include <d3d11.h>
#include <wrl/client.h>


struct VertexShaderToCopyToGpuToGPU
{
	DirectX::XMFLOAT4 colorTint;
	DirectX::XMFLOAT3 offset;
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

	//Gui Helper Methods
	void UpdateImGui(float deltaTime);
	void BuildUI();
	void ChangeWindowColor(float r, float g, float b, float a);

	// Shaders and shader-related constructs
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

	//Gui Variables
	int currentSliderValue;
	int maxSliderValue = 100;
	int minSliderValue = 0;

	bool demoWindowState = true;

	float color[4];

	//Mesh Variables
	std::shared_ptr<Mesh> triangle;
	std::shared_ptr<Mesh> square;
	std::shared_ptr<Mesh> object;

	//constantbuffer
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;

};

