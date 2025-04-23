#include "Game.h"
#include "Graphics.h"
#include "Vertex.h"
#include "Input.h"
#include "PathHelpers.h"
#include "Window.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"

#include <DirectXMath.h>

// Needed for a helper function to load pre-compiled shader files
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Called once per program, after the window and graphics API
// are initialized but before the game loop begins
// --------------------------------------------------------
void Game::Initialize()
{
	// Initialize ImGui itself & platform/renderer backends
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(Window::Handle());
	ImGui_ImplDX11_Init(Graphics::Device.Get(), Graphics::Context.Get());
	// Pick a style (uncomment one of these 3)
	ImGui::StyleColorsDark();

	transform = Transform();
	//ImGui::StyleColorsLight();
	//ImGui::StyleColorsClassic();

	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	CreateGeometry();

	// Set initial graphics API state
	//  - These settings persist until we change them
	//  - Some of these, like the primitive topology & input layout, probably won't change
	//  - Others, like setting shaders, will need to be moved elsewhere later
	{
		// Tell the input assembler (IA) stage of the pipeline what kind of
		// geometric primitives (points, lines or triangles) we want to draw.  
		// Essentially: "What kind of shape should the GPU draw with our vertices?"
		Graphics::Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	//TODO
	//1. Create a map for game entities to names(For organization
	//2. Organize this code (maybe in a different method)
	//3. Only pass in time if they shader wants time


	//Base Shaders
	vs = std::make_shared<SimpleVertexShader>(Graphics::Device, Graphics::Context, FixPath(L"VertexShader.cso").c_str());
	ps =  std::make_shared<SimplePixelShader>(Graphics::Device, Graphics::Context, FixPath(L"PixelShader.cso").c_str());

	//Uv shader
	std::shared_ptr uvPixelShader = std::make_shared<SimplePixelShader>(Graphics::Device, Graphics::Context, FixPath(L"DebugUVPixelShader.cso").c_str());
	
	//Normal Shader
	std::shared_ptr normalPixelShader = std::make_shared<SimplePixelShader>(Graphics::Device, Graphics::Context, FixPath(L"DebugNormalPixelShader.cso").c_str());

	//Custom Shader
	std::shared_ptr customPixelShader = std::make_shared<SimplePixelShader>(Graphics::Device, Graphics::Context, FixPath(L"CustomPixelShader.cso").c_str());

	//Multi Texture Shader
	std::shared_ptr multiTexturePixelShader = std::make_shared<SimplePixelShader>(Graphics::Device, Graphics::Context, FixPath(L"MultiTexturePixelShader.cso").c_str());

	//Sky Box Shaders
	std::shared_ptr skyVertexShader = std::make_shared<SimpleVertexShader>(Graphics::Device, Graphics::Context, FixPath(L"SkyVertexShader.cso").c_str());
	std::shared_ptr skyPixelShader = std::make_shared<SimplePixelShader>(Graphics::Device, Graphics::Context, FixPath(L"SkyPixelShader.cso").c_str());

	DirectX::XMFLOAT4 colorTint(1.0f, 1.0f, 1.0f, 1.0f);
	DirectX::XMFLOAT4 colorTint2(1.0f, 1.0f, 1.0f, 1.0f);
	DirectX::XMFLOAT4 colorTint3(.2f, .2f, .2f, 1.0f);
	DirectX::XMFLOAT4 colorTint4(1.0f, 0.0f, 1.0f, 1.0f);

	//Create Lights
	directionalLight = {};
	pointLight = {};
	spotLight = {};

	//SetLight Properties
	directionalLight.Type = 0;
	directionalLight.Direction = XMFLOAT3(.5f, -1, 0);
	directionalLight.Color = XMFLOAT3(1, 0, 0);
	directionalLight.Intensity = 1;

	pointLight.Type = 1;
	pointLight.Position = XMFLOAT3(5, 2, 0);
	pointLight.Range = 20.0f;
	pointLight.Color = XMFLOAT3(0, 1, 0);
	pointLight.Intensity = 1;

	spotLight.Type = 2;
	spotLight.Position = XMFLOAT3(10, -1, 0);
	spotLight.Direction = XMFLOAT3(0, -1, 0);
	spotLight.Range = 20.0f;
	spotLight.Color = XMFLOAT3(1, 0, 1);
	spotLight.Intensity = 1;
	spotLight.SpotInnerAngle = 15.0f;
	spotLight.SpotOuterAngle = 30.0f;

	//AddLight to List
	lights.push_back(directionalLight);
	lights.push_back(pointLight);
	lights.push_back(spotLight);

	//Shell method for loading all meshes
	MeshLoaderShell();

	//Create Textures

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> brickTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> oakTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> brokenWallTexture;

	//Create Texture With Normal Map
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cobbleStoneTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cobbleStoneNormalTexture;

	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(), L"Assets/Textures/BrickTexture.png", nullptr, brickTexture.GetAddressOf(), (size_t)1000);
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(), L"Assets/Textures/OakTexture.png", nullptr, oakTexture.GetAddressOf(), (size_t)1000);
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(), L"Assets/Textures/BrokenWallTexture.png", nullptr, brokenWallTexture.GetAddressOf(), (size_t)1000);

	//Load Texture With Normal Map
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(), L"Assets/Textures/CobblestoneTexture.png", nullptr, cobbleStoneTexture.GetAddressOf(), (size_t)1000);
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(), L"Assets/NormalMaps/cobblestone_normals.png", nullptr, cobbleStoneNormalTexture.GetAddressOf(), (size_t)1000);

	//Load PBR stuff

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> bronzeAlbedo;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> bronzeMetal;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> bronzeNormals;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> bronzeRoughness;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cobblestoneAlbedo;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cobblestoneMetal;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cobblestoneNormals;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cobblestoneRoughness;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> floorAlbedo;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> floorMetal;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> floorNormals;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> floorRoughness;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> roughAlbedo;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> roughMetal;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> roughNormals;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> roughRoughness;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> paintAlbedo;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> paintMetal;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> paintNormals;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> paintRoughness;


	// Load each texture
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(),L"Assets/Textures/PBR/bronze_albedo.png", nullptr, bronzeAlbedo.GetAddressOf(), (size_t)1000);

	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(),L"Assets/Textures/PBR/bronze_metal.png", nullptr, bronzeMetal.GetAddressOf(), (size_t)1000);

	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(),L"Assets/Textures/PBR/bronze_normals.png", nullptr, bronzeNormals.GetAddressOf(), (size_t)1000);

	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(),L"Assets/Textures/PBR/bronze_roughness.png", nullptr, bronzeRoughness.GetAddressOf(), (size_t)1000);


	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(),L"Assets/Textures/PBR/cobblestone_albedo.png", nullptr, cobblestoneAlbedo.GetAddressOf(), (size_t)1000);

	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(),L"Assets/Textures/PBR/cobblestone_metal.png", nullptr, cobblestoneMetal.GetAddressOf(), (size_t)1000);

	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(),L"Assets/Textures/PBR/cobblestone_normals.png", nullptr, cobblestoneNormals.GetAddressOf(), (size_t)1000);

	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(),L"Assets/Textures/PBR/cobblestone_roughness.png", nullptr, cobblestoneRoughness.GetAddressOf(), (size_t)1000);

	
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(),L"Assets/Textures/PBR/floor_albedo.png", nullptr, floorAlbedo.GetAddressOf(), (size_t)1000);

	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(),L"Assets/Textures/PBR/floor_metal.png", nullptr, floorMetal.GetAddressOf(), (size_t)1000);

	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(),L"Assets/Textures/PBR/floor_normals.png", nullptr, floorNormals.GetAddressOf(), (size_t)1000);

	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(),L"Assets/Textures/PBR/floor_roughness.png", nullptr, floorRoughness.GetAddressOf(), (size_t)1000);


	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(),L"Assets/Textures/PBR/rough_albedo.png", nullptr, roughAlbedo.GetAddressOf(), (size_t)1000);

	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(),L"Assets/Textures/PBR/rough_metal.png", nullptr, roughMetal.GetAddressOf(), (size_t)1000);

	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(),L"Assets/Textures/PBR/rough_normals.png", nullptr, roughNormals.GetAddressOf(), (size_t)1000);

	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(),L"Assets/Textures/PBR/rough_roughness.png", nullptr, roughRoughness.GetAddressOf(), (size_t)1000);

	
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(),L"Assets/Textures/PBR/paint_albedo.png", nullptr, paintAlbedo.GetAddressOf(), (size_t)1000);

	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(),L"Assets/Textures/PBR/paint_metal.png", nullptr, paintMetal.GetAddressOf(), (size_t)1000);

	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(),L"Assets/Textures/PBR/paint_normals.png", nullptr, paintNormals.GetAddressOf(), (size_t)1000);

	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(),L"Assets/Textures/PBR/paint_roughness.png", nullptr, paintRoughness.GetAddressOf(), (size_t)1000);


	//Load SkyBox Textures
	std::vector<std::wstring> textureFiles = {
	L"Assets/Skyboxes/right.png", L"Assets/Skyboxes/left.png",
	L"Assets/Skyboxes/up.png", L"Assets/Skyboxes/down.png",
	L"Assets/Skyboxes/front.png", L"Assets/Skyboxes/back.png"
	};

	//Load Skybox mesh
	std::shared_ptr skyboxMesh = std::make_shared<Mesh>(FixPath("../../Assets/Models/cube.obj").c_str());

	//Create sampler state
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerStateComPtr;

	D3D11_SAMPLER_DESC sampleStateDesc = {};
	sampleStateDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampleStateDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleStateDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleStateDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleStateDesc.MaxAnisotropy = 16;
	sampleStateDesc.MaxLOD = D3D11_FLOAT32_MAX;

	Graphics::Device->CreateSamplerState(&sampleStateDesc, samplerStateComPtr.GetAddressOf());

	//Create skybox
	skyBox = std::make_shared<Sky>(skyboxMesh, samplerStateComPtr, textureFiles, skyPixelShader, skyVertexShader);

	//Create Materials
	CreateMaterial(vs, ps, colorTint, 0);
	CreateMaterial(vs, ps, colorTint2, 0);
	CreateMaterial(vs, ps, colorTint, 0);
	CreateMaterial(vs, multiTexturePixelShader, colorTint, 0);

	//Create Normal Map Material
	CreateMaterial(vs, ps, colorTint, 0);

	//Map Textures and Samplers to materials
	materials[0]->AddTextureSRV("Albedo", bronzeAlbedo);
	materials[0]->AddTextureSRV("NormalMapTexture", bronzeNormals);
	materials[0]->AddTextureSRV("RoughnessMapTexture", bronzeRoughness);
	materials[0]->AddTextureSRV("MetalnessMapTexture", bronzeMetal);
	materials[0]->AddSampler("BasicSampler", samplerStateComPtr);

	materials[1]->AddTextureSRV("Albedo", floorAlbedo);
	materials[1]->AddTextureSRV("NormalMapTexture", floorNormals);
	materials[1]->AddTextureSRV("RoughnessMapTexture", floorRoughness);
	materials[1]->AddTextureSRV("MetalnessMapTexture", floorMetal);
	materials[1]->AddSampler("BasicSampler", samplerStateComPtr);


	materials[2]->AddTextureSRV("Albedo", roughAlbedo);
	materials[2]->AddTextureSRV("NormalMapTexture", roughNormals);
	materials[2]->AddTextureSRV("RoughnessMapTexture", roughRoughness);
	materials[2]->AddTextureSRV("MetalnessMapTexture", roughMetal);
	materials[2]->AddSampler("BasicSampler", samplerStateComPtr);


	materials[3]->AddTextureSRV("Albedo", paintAlbedo);
	materials[3]->AddTextureSRV("NormalMapTexture", paintNormals);
	materials[3]->AddTextureSRV("RoughnessMapTexture", paintRoughness);
	materials[3]->AddTextureSRV("MetalnessMapTexture", paintMetal);
	materials[3]->AddSampler("BasicSampler", samplerStateComPtr);


	//Load Normal Mapped Textures To Material
	materials[4]->AddTextureSRV("Albedo", cobblestoneAlbedo);
	materials[4]->AddTextureSRV("NormalMapTexture", cobblestoneNormals);
	materials[4]->AddTextureSRV("RoughnessMapTexture", cobblestoneRoughness);
	materials[4]->AddTextureSRV("MetalnessMapTexture", cobblestoneMetal);
	materials[4]->AddSampler("BasicSampler", samplerStateComPtr);

	//Create the shadow map
	CreateShadowMap();

	//Add ShadowMapTexture To Materials
	for (auto& e : materials)
	{
		e->AddSampler("ShadowSampler", shadowSampler);
	}


	//// For materials that don't require a normal map:
	//materials[0]->AddTextureSRV("NormalMapTexture", nullptr);
	//materials[1]->AddTextureSRV("NormalMapTexture", nullptr);
	//materials[2]->AddTextureSRV("NormalMapTexture", nullptr);
	//materials[3]->AddTextureSRV("NormalMapTexture", nullptr);


	//Create Game Entities
	CreateGameEntity(*temp_Meshes[0], *materials[0]);
	CreateGameEntity(*temp_Meshes[1], *materials[1]);
	CreateGameEntity(*temp_Meshes[2], *materials[2]);
	CreateGameEntity(*temp_Meshes[3], *materials[3]);
	CreateGameEntity(*temp_Meshes[4], *materials[4]);
	CreateGameEntity(*temp_Meshes[5], *materials[4]);

	CreateCamera(XMFLOAT3(0, 0, -20), 10.0f, 2.0f, XM_PIDIV4, Window::AspectRatio());
	CreateCamera(XMFLOAT3(0, 0, -1), 10.0f, 1.0f, XM_PIDIV4/2, Window::AspectRatio());

	activeCamera = cameraList[0];

	//Move Game Entities

	//Move the TopHat
	gameEntities[1]->GetTransform()->SetPosition(XMFLOAT3(5, 0, 0));

	//Move The Helix
	gameEntities[2]->GetTransform()->SetPosition(XMFLOAT3(10, 0, 0));

	//Move the Cylinder
	gameEntities[3]->GetTransform()->SetPosition(XMFLOAT3(0, -5, 0));

	//Move the cube
	gameEntities[4]->GetTransform()->SetPosition(XMFLOAT3(-5, 0, -10));

	//Move the Floor
	gameEntities[5]->GetTransform()->SetPosition(XMFLOAT3(0, -49, 0));

	//Scale the floor
	gameEntities[5]->GetTransform()->Scale(40, 40, 40);


	//Initialize Window Color and color tint
	ChangeColor(color, 0, 0, 0, 0.0f);

	//Initialize ambient light color
	ambientLightColor = XMFLOAT3(0.1f, 0.1f, 0.25f);
}


// --------------------------------------------------------
// Clean up memory or objects created by this class
// 
// Note: Using smart pointers means there probably won't
//       be much to manually clean up here!
// --------------------------------------------------------
Game::~Game()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

// --------------------------------------------------------
// Handle resizing to match the new window size
//  - Eventually, we'll want to update our 3D camera
// --------------------------------------------------------
void Game::OnResize()
{
	if (activeCamera == nullptr)
	{
		return;
	}
	for (const auto& obj : cameraList)
	{
		obj->UpdateProjectionMatrix(Window::AspectRatio());
	}
}


// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	UpdateImGui(deltaTime);

	BuildUI();

	// Example input checking: Quit if the escape key is pressed
	if (Input::KeyDown(VK_ESCAPE))
		Window::Quit();

	 //Move them on the X
	/*for (int i = 0; i < 5; i++)
	{
		gameEntities[i]->GetTransform()->SetPosition(sinf(totalTime), 0, 0);
		CopyInfoToStruct(i);
	}*/

	activeCamera->Update(deltaTime);

	gameEntities[0]->GetTransform()->SetRotation(totalTime, 0, 0);
}


// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Frame START
	// - These things should happen ONCE PER FRAME
	// - At the beginning of Game::Draw() before drawing *anything*
	{
		// Clear the back buffer (erase what's on screen) and depth buffer
		Graphics::Context->ClearRenderTargetView(Graphics::BackBufferRTV.Get(),	color);
		Graphics::Context->ClearDepthStencilView(Graphics::DepthBufferDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	RenderShadowMap();

	//triangle->Draw();
	for (const auto& obj : gameEntities) 
	{

		obj->GetMaterial()->AddTextureSRV("ShadowMap", shadowSRV);

		obj->Draw(activeCamera, totalTime, lightViewMatrix, lightProjectionMatrix);

		//Set the ambient light color of each object.
		obj->GetMaterial()->GetPS()->SetFloat3("ambientLightColor", ambientLightColor);

		//Sets the lights for the object
		obj->GetMaterial()->PrepareLight(lights);

	}
	skyBox->Draw(activeCamera.get());

	ImGui::Render(); // Turns this frame’s UI into renderable triangles
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()); // Draws it to the screen

	ID3D11ShaderResourceView* nullSRVs[128] = {};
	Graphics::Context->PSSetShaderResources(0, 128, nullSRVs);


	// Frame END
	// - These should happen exactly ONCE PER FRAME
	// - At the very end of the frame (after drawing *everything*)
	{
		// Present at the end of the frame
		bool vsync = Graphics::VsyncState();
		Graphics::SwapChain->Present(
			vsync ? 1 : 0,
			vsync ? 0 : DXGI_PRESENT_ALLOW_TEARING);

		// Re-bind back buffer and depth buffer after presenting
		Graphics::Context->OMSetRenderTargets(
			1,
			Graphics::BackBufferRTV.GetAddressOf(),
			Graphics::DepthBufferDSV.Get());
	}
}

void::Game::UpdateImGui(float deltaTime) 
{
	// Put this all in a helper method that is called from Game::Update()
	// Feed fresh data to ImGui
	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = deltaTime;
	io.DisplaySize.x = (float)Window::Width();
	io.DisplaySize.y = (float)Window::Height();
	// Reset the frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	// Determine new input capture
	Input::SetKeyboardCapture(io.WantCaptureKeyboard);
	Input::SetMouseCapture(io.WantCaptureMouse);
}

void::Game::BuildUI()
{
	//Begins the creation of a new window
	ImGui::Begin("GUI Inspector");

	if (ImGui::CollapsingHeader("Application Details"))
	{
		ImGui::Text("FrameRate: %f fps", ImGui::GetIO().Framerate);

		ImGui::Text("Window Resolution: %dx%d", Window::Width(), Window::Height());

		ImGui::ColorEdit4("Background Color", color);

		ImGui::ColorEdit4("ColorTint", &objectColorTint.x);

		if (ImGui::Button("Show Demo Window"))
		{
			demoWindowState = !demoWindowState;
		}
	}

	if (ImGui::CollapsingHeader("Meshes"))
	{
		int counter = 1;
		for (const auto& obj : gameEntities)
		{
			ImGui::Text(("Entity " + std::to_string(counter) + " Vert Count: %d").c_str(), obj->GetMesh()->GetVertexCount());
			ImGui::Text(("Entity " + std::to_string(counter) + " Index Count: %d").c_str(), obj->GetMesh()->GetIndexCount());

			ImGui::Text(" ");
			
			counter++;
		}
	}

	if (ImGui::CollapsingHeader("Entities")) 
	{
		//CONVERT THIS TO NAMES
		int counter = 1;
		for (const auto& t : gameEntities)
		{
			Transform* trans = t->GetTransform().get();
			//Local Variables
			XMFLOAT3 pos = trans->GetPosition();
			XMFLOAT3 rotation = trans->GetRotation();
			XMFLOAT3 scale = trans->GetScale();
			if (ImGui::CollapsingHeader(("Entity " + std::to_string(counter)).c_str()))
			{
				ImGui::SliderFloat3("Position", &pos.x, -20.0f, 20.0f);
				ImGui::SliderFloat3("Rotation", &rotation.x, -5.0f, 5.0f);
				ImGui::SliderFloat3("Scale", &scale.x, 0.0f, 5.0f);

				trans->SetPosition(pos);
				trans->SetRotation(rotation);
				trans->SetScale(scale);
			}
			counter++;
		}
	}

	if (ImGui::CollapsingHeader("Camera"))
	{
		for (int i = 0; i < 2; i++)
		{
			if (ImGui::Button(("Camera " + std::to_string(i + 1)).c_str()))
			{
				activeCamera = cameraList[i];
			}
		}
	}

	if (ImGui::CollapsingHeader("MaterialInfo"))
	{
		int counter = 1;
		for (auto& t : gameEntities)
		{
			Material* mat = t->GetMaterial().get();

			XMFLOAT4 colorTint = mat->GetColorTint();
			XMFLOAT2 uvScale = mat->GetUVScale();
			XMFLOAT2 uvOffset = mat->GetUVOffset();
			if(ImGui::CollapsingHeader(("Material " + std::to_string(counter)).c_str()))
			{
				ImGui::ColorEdit4("Color Tint: ", &colorTint.x);
				ImGui::SliderFloat2("UV Scale: ", &uvScale.x, .1f, 10);
				ImGui::SliderFloat2("UV Offset: ", &uvOffset.x, 0.0f, 10);

				mat->SetColorTint(colorTint);
				mat->SetUVScale(uvScale.x, uvScale.y);
				mat->SetUVOffset(uvOffset.x, uvOffset.y);
			}
			counter++;
		}
	}

	if (ImGui::CollapsingHeader("Light Info"))
	{
		int counter = 1;
		for (auto& light : lights)
		{
			XMFLOAT3 lightColor = light.Color;

			if (ImGui::CollapsingHeader(("Light " + std::to_string(counter)).c_str()))
			{
				ImGui::ColorEdit3("Color Tint: ", &lightColor.x);
				
			}
			light.Color = lightColor;
			counter++;
		}
	}
	ImGui::Image((ImTextureID)shadowSRV.Get(), ImVec2(512, 512));



	if (demoWindowState)
	{
		ImGui::ShowDemoWindow(&demoWindowState);
	}

	//Ends the creation of a new window
	ImGui::End();

}

void::Game::ChangeColor(float* _color, float r, float g, float b, float a)
{
	_color[0] = r;
	_color[1] = g;
	_color[2] = b;
	_color[3] = a;
}

void Game::CreateGeometry()
{

}

void Game::MeshLoaderShell()
{
	//Load Sphere
	temp_Meshes.push_back(std::make_shared<Mesh>(FixPath("../../Assets/Models/sphere.obj").c_str()));

	//Load Top Hat
	temp_Meshes.push_back(std::make_shared<Mesh>(FixPath("../../Assets/Models/TopHat.obj").c_str()));

	//Load Helix
	temp_Meshes.push_back(std::make_shared<Mesh>(FixPath("../../Assets/Models/helix.obj").c_str()));

	//Load Cylinder
	temp_Meshes.push_back(std::make_shared<Mesh>(FixPath("../../Assets/Models/cylinder.obj").c_str()));

	//Load Cube
	temp_Meshes.push_back(std::make_shared<Mesh>(FixPath("../../Assets/Models/cube.obj").c_str()));

	//Load Second Cube as the Floor
	temp_Meshes.push_back(std::make_shared<Mesh>(FixPath("../../Assets/Models/cube.obj").c_str()));


}

/// <summary>
/// Creates Material and Adds It To Materials List
/// </summary>
/// <param name="_vs"></param>
/// <param name="_ps"></param>
/// <param name="_colorTint"></param>
void Game::CreateMaterial(std::shared_ptr<SimpleVertexShader> _vs, std::shared_ptr<SimplePixelShader> _ps, DirectX::XMFLOAT4 _colorTint, float _roughness)
{
	materials.push_back(std::make_shared<Material>(_vs, _ps, _colorTint, _roughness));
}

/// <summary>
/// Creates an object, and adds it to the game entities list
/// </summary>
/// <param name="mesh"></param>
/// <param name="mat"></param>
void Game::CreateGameEntity(Mesh mesh, Material mat)
{
	gameEntities.push_back(std::make_shared<GameEntity>(mesh, mat));
}

/// <summary>
/// Creates a camera and adds it to our camera list
/// </summary>
/// <param name="pos"></param>
/// <param name="moveSpeed"></param>
/// <param name="lookSpeed"></param>
/// <param name="fov"></param>
/// <param name="aspectRatio"></param>
void Game::CreateCamera(DirectX::XMFLOAT3 pos, float moveSpeed, float lookSpeed, float fov, float aspectRatio)
{
	cameraList.push_back(std::make_shared<Camera>(pos, moveSpeed, lookSpeed, fov, aspectRatio));
}

void Game::CreatePBRMaterial()
{

}

void Game::CreateShadowMap()
{
	// Create the actual texture that will be the shadow map
	D3D11_TEXTURE2D_DESC shadowDesc = {};
	shadowDesc.Width = 1024; // Ideally a power of 2 (like 1024)
	shadowDesc.Height = 1024; // Ideally a power of 2 (like 1024)
	shadowDesc.ArraySize = 1;
	shadowDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	shadowDesc.CPUAccessFlags = 0;
	shadowDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	shadowDesc.MipLevels = 1;
	shadowDesc.MiscFlags = 0;
	shadowDesc.SampleDesc.Count = 1;
	shadowDesc.SampleDesc.Quality = 0;
	shadowDesc.Usage = D3D11_USAGE_DEFAULT;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> shadowTexture;
	Graphics::Device->CreateTexture2D(&shadowDesc, 0, shadowTexture.GetAddressOf());

	// Create the depth/stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC shadowDSDesc = {};
	shadowDSDesc.Format = DXGI_FORMAT_D32_FLOAT;
	shadowDSDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	shadowDSDesc.Texture2D.MipSlice = 0;
	Graphics::Device->CreateDepthStencilView(
		shadowTexture.Get(),
		&shadowDSDesc,
		shadowDSV.GetAddressOf());
	// Create the SRV for the shadow map
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	Graphics::Device->CreateShaderResourceView(
		shadowTexture.Get(),
		&srvDesc,
		shadowSRV.GetAddressOf());

	XMVECTOR lightDirection = XMLoadFloat3(&lights[0].Direction);


	//Set Up Lights For Shadow Map
	XMMATRIX lightView = XMMatrixLookToLH(
		-lightDirection * 20, // Position: "Backing up" 20 units from origin
		lightDirection, // Direction: light's direction
		XMVectorSet(0, 1, 0, 0)); // Up: World up vector (Y axis)

	float lightProjectionSize = 60.0f; // Tweak for your scene!
	XMMATRIX lightProjection = XMMatrixOrthographicLH(
		lightProjectionSize,
		lightProjectionSize,
		1.0f,
		100.0f);

	D3D11_RASTERIZER_DESC shadowRastDesc = {};
	shadowRastDesc.FillMode = D3D11_FILL_SOLID;
	shadowRastDesc.CullMode = D3D11_CULL_BACK;
	shadowRastDesc.DepthClipEnable = true;
	shadowRastDesc.DepthBias = 1000; // Min. precision units, not world units!
	shadowRastDesc.SlopeScaledDepthBias = 1.0f; // Bias more based on slope
	Graphics::Device->CreateRasterizerState(&shadowRastDesc, &shadowRasterizer);

	D3D11_SAMPLER_DESC shadowSampDesc = {};
	shadowSampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	shadowSampDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
	shadowSampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.BorderColor[0] = 1.0f; // Only need the first component
	Graphics::Device->CreateSamplerState(&shadowSampDesc, &shadowSampler);


	XMStoreFloat4x4(&lightViewMatrix, lightView);
	XMStoreFloat4x4(&lightProjectionMatrix, lightProjection);


	shadowVS = std::make_shared<SimpleVertexShader>(Graphics::Device, Graphics::Context, FixPath(L"ShadowMapVS.cso").c_str());

}

void Game::RenderShadowMap()
{
	Graphics::Context->ClearDepthStencilView(shadowDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	ID3D11RenderTargetView* nullRTV{};
	Graphics::Context->OMSetRenderTargets(1, &nullRTV, shadowDSV.Get());
	Graphics::Context->PSSetShader(0, 0, 0);

	D3D11_VIEWPORT viewport = {};
	viewport.Width = (float)1024;
	viewport.Height = (float)1024;
	viewport.MaxDepth = 1.0f;
	Graphics::Context->RSSetViewports(1, &viewport);
	Graphics::Context->RSSetState(shadowRasterizer.Get());

	shadowVS->SetShader();
	shadowVS->SetMatrix4x4("view", lightViewMatrix);
	shadowVS->SetMatrix4x4("projection", lightProjectionMatrix);
	// Loop and draw all entities
	for (auto& e : gameEntities)
	{
		shadowVS->SetMatrix4x4("world", e->GetTransform()->GetWorldMatrix());
		shadowVS->CopyAllBufferData();
		// Draw the mesh directly to avoid the entity's material
		// Note: Your code may differ significantly here!
		e->GetMesh()->Draw();
	}

	viewport.Width = (float)Window::Width();
	viewport.Height = (float)Window::Height();
	Graphics::Context->RSSetViewports(1, &viewport);
	Graphics::Context->OMSetRenderTargets(
		1,
		Graphics::BackBufferRTV.GetAddressOf(),
		Graphics::DepthBufferDSV.Get());
	Graphics::Context->RSSetState(0);
}




