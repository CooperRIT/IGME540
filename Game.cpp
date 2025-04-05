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
	directionalLight.Direction = XMFLOAT3(1, 0, 0);
	directionalLight.Color = XMFLOAT3(1, 0.0f, 0.0f);
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

	//Load SkyBox Textures
	std::vector<std::wstring> textureFiles = {
	L"Assets/Skyboxes/right.png", L"Assets/Skyboxes/left.png",
	L"Assets/Skyboxes/up.png", L"Assets/Skyboxes/down.png",
	L"Assets/Skyboxes/back.png", L"Assets/Skyboxes/front.png"
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
	sampleStateDesc.MaxLOD = D3D11_FLOAT32_MAX;

	Graphics::Device.Get()->CreateSamplerState(&sampleStateDesc, samplerStateComPtr.GetAddressOf());

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
	materials[0]->AddTextureSRV("SurfaceTexture", oakTexture);
	materials[0]->AddSampler("BasicSampler", samplerStateComPtr);

	materials[1]->AddTextureSRV("SurfaceTexture", oakTexture);
	materials[1]->AddSampler("BasicSampler", samplerStateComPtr);

	materials[2]->AddTextureSRV("SurfaceTexture", brickTexture);
	materials[2]->AddSampler("BasicSampler", samplerStateComPtr);

	materials[3]->AddTextureSRV("SurfaceTexture", brickTexture);
	materials[3]->AddTextureSRV("SecondaryTexture", brokenWallTexture);
	materials[3]->AddSampler("BasicSampler", samplerStateComPtr);

	//Load Normal Mapped Textures To Material
	materials[4]->AddTextureSRV("SurfaceTexture", cobbleStoneTexture);
	materials[4]->AddTextureSRV("NormalMapTexture", cobbleStoneNormalTexture);
	materials[4]->AddSampler("BasicSampler", samplerStateComPtr);

	// For materials that don't require a normal map:
	materials[0]->AddTextureSRV("NormalMapTexture", nullptr);
	materials[1]->AddTextureSRV("NormalMapTexture", nullptr);
	materials[2]->AddTextureSRV("NormalMapTexture", nullptr);
	materials[3]->AddTextureSRV("NormalMapTexture", nullptr);


	//Create Game Entities
	CreateGameEntity(*temp_Meshes[0], *materials[0]);
	CreateGameEntity(*temp_Meshes[1], *materials[1]);
	CreateGameEntity(*temp_Meshes[2], *materials[2]);
	CreateGameEntity(*temp_Meshes[3], *materials[3]);
	CreateGameEntity(*temp_Meshes[4], *materials[4]);

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

	//triangle->Draw();
	for (const auto& obj : gameEntities) 
	{
		obj->Draw(activeCamera, totalTime);

		//Set the ambient light color of each object.
		obj->GetMaterial()->GetPS()->SetFloat3("ambientLightColor", ambientLightColor);

		//Sets the lights for the object
		obj->GetMaterial()->PrepareLight(lights);
	}
	skyBox->Draw(activeCamera.get());

	ImGui::Render(); // Turns this frame’s UI into renderable triangles
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()); // Draws it to the screen



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




