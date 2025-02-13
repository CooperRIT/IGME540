#pragma once

#include "Mesh.h"
#include <memory>
#include <DirectXMath.h>
#include "Transform.h"
#include "GameEntity.h"

#include <d3d11.h>
#include <wrl/client.h>
#include <vector>


struct VertexShaderToCopyToGpuToGPU
{
	DirectX::XMFLOAT4 colorTint;
	DirectX::XMFLOAT4X4 offset;
};

//This is for ImGUI
struct EntityInformation 
{
	/*float objectPosition[3];  
	float objectRotation[3];
	float objectScale[3];*/

	DirectX::XMFLOAT3 objectPosition;
	DirectX::XMFLOAT3 objectRotation;
	DirectX::XMFLOAT3 objectScale;
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
	void CopyInfoToStruct(int index);

	// Shaders and shader-related constructs
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

	//Gui Variables
	int currentSliderValue;
	int maxSliderValue = 100;
	int minSliderValue = 0;

	bool demoWindowState = false;

	DirectX::XMFLOAT4 objectColorTint;
	float objectOffset[3];

	float color[4];

	//EntityInfo List
	std::vector <EntityInformation> entityInfo;

	//Mesh List
	std::vector<std::shared_ptr<GameEntity>> gameEntities;

	//constantbuffer
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;

	//Game Entities
	std::shared_ptr<GameEntity> triangle;
};



