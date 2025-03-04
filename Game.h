#pragma once

#include "Mesh.h"
#include <memory>
#include <DirectXMath.h>
#include "Transform.h"
#include "GameEntity.h"
#include "Camera.h"
#include "SimpleShader.h"
#include "Material.h"

#include <d3d11.h>
#include <wrl/client.h>
#include <vector>

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

	Transform transform;

	//Gui Helper Methods
	void UpdateImGui(float deltaTime);
	void BuildUI();
	void ChangeColor(float* _color, float r, float g, float b, float a);
	void CopyXMFloatToArray(DirectX::XMFLOAT3 xmFloat, float* floatArray);

	//Gui Variables
	int currentSliderValue;
	int maxSliderValue = 100;
	int minSliderValue = 0;

	bool demoWindowState = false;

	DirectX::XMFLOAT4 objectColorTint;
	float objectOffset[3];

	float color[4];

	//Mesh List
	std::vector<std::shared_ptr<GameEntity>> gameEntities;

	//Camera List
	std::vector<std::shared_ptr<Camera>> cameraList;

	//Game Entities
	std::shared_ptr<GameEntity> triangle;

	//camera
	std::shared_ptr<Camera> activeCamera;

	//Shaders
	std::shared_ptr<SimpleVertexShader> vs;
	std::shared_ptr<SimplePixelShader> ps;

	//Materials
	std::vector<std::shared_ptr<Material>> materials;
};



