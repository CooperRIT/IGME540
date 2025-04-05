#pragma once

#include "Mesh.h"
#include <memory>
#include <DirectXMath.h>
#include "Transform.h"
#include "GameEntity.h"
#include "Camera.h"
#include "SimpleShader.h"
#include "Material.h"
#include "WICTextureLoader.h"
#include "Lights.h"
#include "Sky.h"

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

	//Game Class Helper Methods
	void CreateGeometry();
	void MeshLoaderShell();
	void CreateMaterial(std::shared_ptr<SimpleVertexShader> _vs, std::shared_ptr<SimplePixelShader> _ps, DirectX::XMFLOAT4 _colorTint, float _roughness);
	void CreateGameEntity(Mesh mesh, Material mat);
	void CreateCamera(DirectX::XMFLOAT3 pos, float moveSpeed, float lookSpeed, float fov, float aspectRatio);

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

	//Mesh List For temp storage
	std::vector<std::shared_ptr<Mesh>> temp_Meshes;

	//Lighting
	DirectX::XMFLOAT3 ambientLightColor;

	std::vector<Light> lights;

	Light directionalLight;

	Light pointLight;

	Light spotLight;

	//SkyBox
	std::shared_ptr<Sky> skyBox;
};



